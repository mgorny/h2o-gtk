/* h2o-gtk+ -- GTK+ UI to libh2o
 * (c) 2012 Michał Górny
 * Released under the terms of the 2-clause BSD license
 */

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif

#include "datainputoutput.hxx"

#include <cassert>
#include <stdexcept>

FunctionChoiceComboBox::FunctionChoiceComboBox()
{
	append_text("f(p, T)");
	append_text("f(p, h)");
	append_text("f(p, s)");
	append_text("f(p, x)");
	append_text("f(T, x)");
	append_text("f(h, s)");
	append_text("f(\317\201, T)");

	set_active(0);
}

enum Function FunctionChoiceComboBox::get_function()
{
	return static_cast<enum Function>(get_active_row_number());
}

DataInputBase::DataInputBase(Gtk::Table& t, int first_row)
	: _parent(t), _first_row(first_row),
	p("_p", "MPa", 1E-4, 100, 0.1, 5, 4, 10),
	T("_T", "K", 273.15, 2273.15, 1, 50, 2, 773.15),
	v("_v", "m³/kg", 0, 1E17, 0.01, 1, 6),
	u("_u", "kJ/kg", 0, 6400, 10, 200, 2),
	h("_h", "kJ/kg", 0, 7500, 10, 200, 2),
	s("_s", "kJ/kgK", 0, 28, 0.04, 0.2, 3),
	x("_x", "[-]", 0, 1, 0.004, 0.02, 3),
	rho("\317\201", "kg/m\302\263", 0, 1050, 5, 40, 2),
	func_label("_f", Gtk::ALIGN_END, Gtk::ALIGN_CENTER, true)
{
	func_label.set_mnemonic_widget(func_chooser);
	t.attach(func_label, 0, 1, first_row + 0, first_row + 1);
	t.attach(func_chooser, 1, 2, first_row + 0, first_row + 1);

	region_label.set_padding(0, 10);
	region_label.set_alignment(Gtk::ALIGN_CENTER, Gtk::ALIGN_END);
	t.attach(region_label, 0, 3, first_row + 3, first_row + 4);

	func_chooser.signal_changed().connect(
			sigc::mem_fun(*this, &DataInputBase::reorder_fields));
}

void DataInputBase::remove_fields()
{
	conn1.disconnect();
	conn2.disconnect();

	p.remove_from_table(_parent);
	T.remove_from_table(_parent);

	v.remove_from_table(_parent);
	u.remove_from_table(_parent);
	h.remove_from_table(_parent);
	s.remove_from_table(_parent);
	x.remove_from_table(_parent);
	rho.remove_from_table(_parent);
}

void DataInputBase::set_fields(DataEntryPair& in1, DataEntryPair& in2,
		DataEntryPair& out1, DataEntryPair& out2,
		DataEntryPair& out3, DataEntryPair& out4,
		DataEntryPair& out5)
{
	in1.add_to_table(_parent, _first_row + 1);
	in2.add_to_table(_parent, _first_row + 2);
	in1.enable();
	in2.enable();

	conn1 = in1.signal_value_changed().connect(
			sigc::mem_fun(*this, &DataInputBase::recalc));
	conn2 = in2.signal_value_changed().connect(
			sigc::mem_fun(*this, &DataInputBase::recalc));
}

typedef double (h2o::H2O::*MediumProperty)() const;

static inline void update_field(DataEntryPair& dest, h2o::H2O& medium, MediumProperty prop)
{
	try
	{
		double v = (medium.*prop)();
		dest.set_readonly_value(v);
		dest.set_sensitive(true);
	}
	catch (std::runtime_error)
	{
		dest.set_sensitive(false);
	};
}

static const double scrit = 4.41202148223476; // [kJ/kgK]

static inline void r3_preset_x(DataEntryPair& x, DataEntryPair& s)
{
	if (s.get_value() >= scrit)
		x.set_readonly_value(1);
	else
		x.set_readonly_value(0);
}

h2o::H2O DataInputBase::get_h2o()
{
	h2o::H2O medium;

	switch (func_chooser.get_function())
	{
		case f_pT:
			medium = h2o::H2O(p.get_value(), T.get_value());
			break;
		case f_ph:
			medium = h2o::H2O::ph(p.get_value(), h.get_value());
			break;
		case f_ps:
			medium = h2o::H2O::ps(p.get_value(), s.get_value());
			break;
		case f_px:
			medium = h2o::H2O::px(p.get_value(), x.get_value());
			break;
		case f_Tx:
			medium = h2o::H2O::Tx(T.get_value(), x.get_value());
			break;
		case f_hs:
			medium = h2o::H2O::hs(h.get_value(), s.get_value());
			break;
		case f_rhoT:
			medium = h2o::H2O::rhoT(rho.get_value(), T.get_value());
			break;
	}

	return medium;
}

void DataInputBase::recalc()
{
	h2o::H2O medium;

	try
	{
		medium = get_h2o();

		const char* label_text;
		switch (medium.region())
		{
			case 1:
				label_text = "Region 1 (saturated water)";
				break;
			case 2:
				label_text = "Region 2 (dry steam)";
				break;
			case 3:
				label_text = "Region 3 (supercritical)";
				break;
			case 4:
				label_text = "Region 4 (wet steam)";
				break;
			case 5:
				label_text = "Region 5 (dry steam)";
				break;
			default:
				label_text = "Region unknown/invalid";
		}
		region_label.set_text(label_text);
	}
	catch (std::range_error)
	{
		region_label.set_text("Out-of-range");
	}
	catch (std::runtime_error)
	{
		region_label.set_text("Not implemented");
	};

	update_field(u, medium, &h2o::H2O::u);

	data_changed.emit(&medium, 1);
}

void DataInputBase::reorder_fields()
{
	remove_fields();

	switch (func_chooser.get_function())
	{
		case f_pT:
			set_fields(p, T, v, u, h, s, x);
			break;
		case f_ph:
			set_fields(p, h, T, v, u, s, x);
			break;
		case f_ps:
			set_fields(p, s, T, v, u, h, x);
			break;
		case f_px:
			set_fields(p, x, T, v, u, h, s);
			break;
		case f_Tx:
			set_fields(T, x, p, v, u, h, s);
			break;
		case f_hs:
			set_fields(h, s, p, T, v, u, x);
			break;
		case f_rhoT:
			rho.set_value(1/v.get_value());
			set_fields(rho, T, p, u, h, s, x);
			break;
	}

	recalc();
}

DataInputBase::data_changed_sig DataInputBase::signal_data_changed()
{
	return data_changed;
}

DataInput::DataInput(Gtk::Table& t, int first_row)
	: DataInputBase(t, first_row)
{
	set_fields(p, T, v, u, h, s, x);
}

DataInputOutput::DataInputOutput(Gtk::Table& t, int first_row)
	: DataInputBase(t, first_row)
{
	signal_data_changed().connect(
			sigc::mem_fun(*this, &DataInputOutput::recalc_for));

	set_fields(p, T, v, u, h, s, x);
}

void DataInputOutput::set_fields(DataEntryPair& in1, DataEntryPair& in2,
		DataEntryPair& out1, DataEntryPair& out2,
		DataEntryPair& out3, DataEntryPair& out4,
		DataEntryPair& out5)
{
	DataInputBase::set_fields(in1, in2, out1, out2, out3, out4, out5);

	out1.add_to_table(_parent, _first_row + 4);
	out2.add_to_table(_parent, _first_row + 5);
	out3.add_to_table(_parent, _first_row + 6);
	out4.add_to_table(_parent, _first_row + 7);
	out5.add_to_table(_parent, _first_row + 8);
	out1.disable();
	out2.disable();
	out3.disable();
	out4.disable();
	out5.disable();
}

void DataInputOutput::recalc_for(h2o::H2O* data, int len)
{
	assert(len == 1);

	h2o::H2O& medium = data[0];

	switch (func_chooser.get_function())
	{
		case f_pT:
			update_field(h, medium, &h2o::H2O::h);
			update_field(s, medium, &h2o::H2O::s);
			if (medium.region() == 3)
				r3_preset_x(x, s);
			update_field(x, medium, &h2o::H2O::x);
			update_field(v, medium, &h2o::H2O::v);
			break;
		case f_ph:
			update_field(T, medium, &h2o::H2O::T);
			update_field(s, medium, &h2o::H2O::s);
			if (medium.region() == 3)
				r3_preset_x(x, s);
			update_field(x, medium, &h2o::H2O::x);
			update_field(v, medium, &h2o::H2O::v);
			break;
		case f_ps:
			update_field(T, medium, &h2o::H2O::T);
			update_field(h, medium, &h2o::H2O::h);
			if (medium.region() == 3)
				r3_preset_x(x, s);
			update_field(x, medium, &h2o::H2O::x);
			update_field(v, medium, &h2o::H2O::v);
			break;
		case f_px:
			update_field(T, medium, &h2o::H2O::T);
			update_field(h, medium, &h2o::H2O::h);
			update_field(s, medium, &h2o::H2O::s);
			update_field(v, medium, &h2o::H2O::v);
			break;
		case f_Tx:
			update_field(p, medium, &h2o::H2O::p);
			update_field(h, medium, &h2o::H2O::h);
			update_field(s, medium, &h2o::H2O::s);
			update_field(v, medium, &h2o::H2O::v);
			break;
		case f_hs:
			update_field(p, medium, &h2o::H2O::p);
			update_field(T, medium, &h2o::H2O::T);
			if (medium.region() == 3)
				r3_preset_x(x, s);
			update_field(x, medium, &h2o::H2O::x);
			update_field(v, medium, &h2o::H2O::v);
			break;
		case f_rhoT:
			update_field(p, medium, &h2o::H2O::p);
			update_field(h, medium, &h2o::H2O::h);
			update_field(s, medium, &h2o::H2O::s);
			if (medium.region() == 3)
				r3_preset_x(x, s);
			update_field(x, medium, &h2o::H2O::x);
	}
}

LockedDataInputOutput::LockedDataInputOutput(Gtk::Table& t, int first_row,
		Function locked_func, double start_p)
	: DataInputOutput(t, first_row - 1)
{
	_parent.remove(func_label);
	_parent.remove(func_chooser);

	func_chooser.set_active(locked_func);
	user_entry->set_value(start_p);
}

void LockedDataInputOutput::set_fields(
		DataEntryPair& in1, DataEntryPair& in2,
		DataEntryPair& out1, DataEntryPair& out2,
		DataEntryPair& out3, DataEntryPair& out4,
		DataEntryPair& out5)
{
	DataInputOutput::set_fields(in1, in2, out1, out2, out3, out4, out5);

	in2.disable();
	user_entry = &in1;
	controlled_entry = &in2;
}

void LockedDataInputOutput::set_user_value_range(double min, double max)
{
	user_entry->set_range(min, max);
}

void LockedDataInputOutput::set_controlled_value(double val)
{
	controlled_entry->set_readonly_value(val);
}
