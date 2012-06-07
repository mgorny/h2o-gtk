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
	set_tooltip_text("Input parameters choice");
}

enum Function FunctionChoiceComboBox::get_function()
{
	return static_cast<enum Function>(get_active_row_number());
}

DataIOBase::DataIOBase(Gtk::Table& t)
	: _parent(t),
	p(10),
	T(773.15),
	func_label("_f", Gtk::ALIGN_END, Gtk::ALIGN_CENTER, true)
{
	func_chooser.signal_changed().connect(
			sigc::mem_fun(*this, &DataIOBase::reorder_fields));
}

void DataIOBase::remove_fields()
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

void DataIOBase::set_fields(DataEntryPair& in1, DataEntryPair& in2,
		DataEntryPair& out1, DataEntryPair& out2,
		DataEntryPair& out3, DataEntryPair& out4,
		DataEntryPair& out5)
{
	conn1 = in1.signal_value_changed().connect(
			sigc::mem_fun(*this, &DataIOBase::recalc));
	conn2 = in2.signal_value_changed().connect(
			sigc::mem_fun(*this, &DataIOBase::recalc));
}

static const double scrit = 4.41202148223476; // [kJ/kgK]

static double r3_guess_x(double s)
{
	if (s >= scrit)
		return 1;
	else
		return 0;
}

static void set_guessed_x(DataEntryPair& x, double s)
{
	x.set_readonly_value(r3_guess_x(s));
	x.set_sensitive(false);
}

h2o::H2O DataIOBase::get_h2o()
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

void DataIOBase::recalc()
{
	h2o::H2O medium;

	try
	{
		medium = get_h2o();
	}
	catch (std::range_error)
	{
		// leave it uninitialized
	};

	data_changed.emit(&medium, 1);
}

void DataIOBase::reorder_fields()
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
			set_fields(p, x, v, u, h, s, T);
			break;
		case f_Tx:
			set_fields(T, x, v, u, h, s, p);
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

DataIOBase::data_changed_sig DataIOBase::signal_data_changed()
{
	return data_changed;
}

void DataInputBase::set_fields(DataEntryPair& in1, DataEntryPair& in2,
		DataEntryPair& out1, DataEntryPair& out2,
		DataEntryPair& out3, DataEntryPair& out4,
		DataEntryPair& out5)
{
	DataIOBase::set_fields(in1, in2, out1, out2, out3, out4, out5);

	in1.add_to_table(_parent, _first_row + 1, _first_col);
	in2.add_to_table(_parent, _first_row + 2, _first_col);
	in1.enable();
	in2.enable();
}

DataInputBase::DataInputBase(Gtk::Table& t, int first_row, int first_col)
	: DataIOBase(t), _first_row(first_row), _first_col(first_col)
{

	func_label.set_mnemonic_widget(func_chooser);
	t.attach(func_label, first_col, first_col + 1,
			first_row + 0, first_row + 1);
	t.attach(func_chooser, first_col + 1, first_col + 2,
			first_row + 0, first_row + 1);
}

void DataInput::store_h2o(h2o::H2O* data, int len)
{
	assert(len == 1);

	last_h2o = data[0];
}

void DataInput::set_fields(DataEntryPair& in1, DataEntryPair& in2,
		DataEntryPair& out1, DataEntryPair& out2,
		DataEntryPair& out3, DataEntryPair& out4,
		DataEntryPair& out5)
{
	if (last_h2o.initialized())
	{
		switch (func_chooser.get_function())
		{
			case f_pT:
				p.set_value(last_h2o.p());
				T.set_value(last_h2o.T());
				break;
			case f_ph:
				p.set_value(last_h2o.p());
				h.set_value(last_h2o.h());
				break;
			case f_ps:
				p.set_value(last_h2o.p());
				s.set_value(last_h2o.s());
				break;
			case f_px:
				p.set_value(last_h2o.p());
				if (last_h2o.region() == h2o::Region::R3)
					set_guessed_x(x, last_h2o.s());
				else
					x.set_value(last_h2o.x());
				break;
			case f_Tx:
				T.set_value(last_h2o.T());
				if (last_h2o.region() == h2o::Region::R3)
					set_guessed_x(x, last_h2o.s());
				else
					x.set_value(last_h2o.x());
				break;
			case f_hs:
				h.set_value(last_h2o.h());
				s.set_value(last_h2o.s());
				break;
			case f_rhoT:
				rho.set_value(last_h2o.rho());
				T.set_value(last_h2o.T());
				break;
		}
	}

	DataInputBase::set_fields(in1, in2, out1, out2, out3, out4, out5);
}

DataInput::DataInput(Gtk::Table& t, int first_row, int first_col)
	: DataIOBase(t),
	DataInputBase(t, first_row, first_col)
{
	signal_data_changed().connect(
			sigc::mem_fun(*this, &DataInput::store_h2o));

	set_fields(p, T, v, u, h, s, x);
}

static void disable_fields(
		DataEntryPair& f1, DataEntryPair& f2,
		DataEntryPair& f3, DataEntryPair& f4)
{
	f1.set_sensitive(false);
	f2.set_sensitive(false);
	f3.set_sensitive(false);
	f4.set_sensitive(false);
}

void DataOutputBase::recalc_for(h2o::H2O* data, int len)
{
	assert(len == 1);

	h2o::H2O& medium = data[0];

	if (!medium.initialized())
	{
		switch (func_chooser.get_function())
		{
			case f_pT:
				disable_fields(h, s, x, v);
				break;
			case f_ph:
				disable_fields(T, s, x, v);
				break;
			case f_ps:
				disable_fields(T, h, x, v);
				break;
			case f_px:
				disable_fields(T, h, s, v);
				break;
			case f_Tx:
				disable_fields(p, h, s, v);
				break;
			case f_hs:
				disable_fields(p, T, x, v);
				break;
			case f_rhoT:
				disable_fields(p, h, s, x);
		}

		u.set_sensitive(false);
	}
	else
	{
		switch (func_chooser.get_function())
		{
			case f_pT:
				h.set_readonly_value(medium.h());
				s.set_readonly_value(medium.s());
				if (medium.region() == h2o::Region::R3)
					set_guessed_x(x, s.get_value());
				else
					x.set_readonly_value(medium.x());
				v.set_readonly_value(medium.v());
				break;
			case f_ph:
				T.set_readonly_value(medium.T());
				s.set_readonly_value(medium.s());
				if (medium.region() == h2o::Region::R3)
					set_guessed_x(x, s.get_value());
				else
					x.set_readonly_value(medium.x());
				v.set_readonly_value(medium.v());
				break;
			case f_ps:
				T.set_readonly_value(medium.T());
				h.set_readonly_value(medium.h());
				if (medium.region() == h2o::Region::R3)
					set_guessed_x(x, s.get_value());
				else
					x.set_readonly_value(medium.x());
				v.set_readonly_value(medium.v());
				break;
			case f_px:
				T.set_readonly_value(medium.T());
				h.set_readonly_value(medium.h());
				s.set_readonly_value(medium.s());
				v.set_readonly_value(medium.v());
				break;
			case f_Tx:
				p.set_readonly_value(medium.p());
				h.set_readonly_value(medium.h());
				s.set_readonly_value(medium.s());
				v.set_readonly_value(medium.v());
				break;
			case f_hs:
				p.set_readonly_value(medium.p());
				T.set_readonly_value(medium.T());
				if (medium.region() == h2o::Region::R3)
					set_guessed_x(x, s.get_value());
				else
					x.set_readonly_value(medium.x());
				v.set_readonly_value(medium.v());
				break;
			case f_rhoT:
				p.set_readonly_value(medium.p());
				h.set_readonly_value(medium.h());
				s.set_readonly_value(medium.s());
				if (medium.region() == h2o::Region::R3)
					set_guessed_x(x, s.get_value());
				else
					x.set_readonly_value(medium.x());
		}

		u.set_readonly_value(medium.u());
	}
}

void DataOutputBase::set_fields(DataEntryPair& in1, DataEntryPair& in2,
		DataEntryPair& out1, DataEntryPair& out2,
		DataEntryPair& out3, DataEntryPair& out4,
		DataEntryPair& out5)
{
	out1.add_to_table(_parent, _first_row, _first_col);
	out2.add_to_table(_parent, _first_row + 1, _first_col);
	out3.add_to_table(_parent, _first_row + 2, _first_col);
	out4.add_to_table(_parent, _first_row + 3, _first_col);
	out1.disable();
	out2.disable();
	out3.disable();
	out4.disable();

	input_entry1 = &in1;
	input_entry2 = &in2;
}

DataOutputBase::DataOutputBase(Gtk::Table& t, int first_row, int first_col)
	: DataIOBase(t), _first_row(first_row), _first_col(first_col)
{
	signal_data_changed().connect(
			sigc::mem_fun(*this, &DataOutputBase::recalc_for));
}

void DataOutput::set_fields(DataEntryPair& in1, DataEntryPair& in2,
		DataEntryPair& out1, DataEntryPair& out2,
		DataEntryPair& out3, DataEntryPair& out4,
		DataEntryPair& out5)
{
	DataIOBase::set_fields(in1, in2, out1, out2, out3, out4, out5);
	DataOutputBase::set_fields(in1, in2, out1, out2, out3, out4, out5);
}

void DataOutputBase::set_function(Function f)
{
	func_chooser.set_active(f);
}

void DataOutputBase::set_value1(double val)
{
	input_entry1->set_readonly_value(val);
}

void DataOutputBase::set_value2(double val)
{
	input_entry2->set_readonly_value(val);
}

DataOutput::DataOutput(Gtk::Table& t, int first_row,
		Function locked_func,
		double start_val1, double start_val2,
		int first_col)
	: DataIOBase(t),
	DataOutputBase(t, first_row, first_col)
{
	set_function(locked_func);
	set_value1(start_val1);
	set_value2(start_val2);
}

void DataOutputWithRegion::recalc_for(h2o::H2O* data, int len)
{
	assert(len == 1);

	h2o::H2O& medium = data[0];

	const char* label_text;
	switch (medium.region())
	{
		case h2o::Region::R1:
			label_text = "Region 1 (sat. water)";
			break;
		case h2o::Region::R2:
			label_text = "Region 2 (dry steam)";
			break;
		case h2o::Region::R3:
			label_text = "Region 3 (supercrit.)";
			break;
		case h2o::Region::R4:
			label_text = "Region 4 (wet steam)";
			break;
		case h2o::Region::R5:
			label_text = "Region 5 (dry steam)";
			break;
		default:
			label_text = "Out-of-range";
	}
	region_label.set_text(label_text);

	if (medium.initialized())
	{
		switch (func_chooser.get_function())
		{
			case f_pT:
			case f_ph:
			case f_ps:
			case f_hs:
			case f_rhoT:
				if (medium.region() == h2o::Region::R3)
					set_guessed_x(x, s.get_value());
				else
					x.set_readonly_value(medium.x());
				break;
			case f_px:
			case f_Tx:
				break;
		}
	}
}

void DataOutputWithRegion::set_fields(DataEntryPair& in1, DataEntryPair& in2,
		DataEntryPair& out1, DataEntryPair& out2,
		DataEntryPair& out3, DataEntryPair& out4,
		DataEntryPair& out5)
{
	DataOutputBase::set_fields(in1, in2, out1, out2, out3, out4, out5);

	out5.add_to_table(_parent, _first_row + 4, _first_col);
	out5.disable();
}

DataOutputWithRegion::DataOutputWithRegion(Gtk::Table& t, int first_row, int first_col)
	: DataIOBase(t),
	DataOutputBase(t, first_row + 1, first_col)
{
	region_label.set_padding(0, 10);
	region_label.set_alignment(Gtk::ALIGN_CENTER, Gtk::ALIGN_END);
	t.attach(region_label, first_col, first_col + 3, first_row, first_row + 1);

	signal_data_changed().connect(
			sigc::mem_fun(*this, &DataOutputWithRegion::recalc_for));
}

DataInputOutput::DataInputOutput(Gtk::Table& t, int first_row, int first_col)
	: DataIOBase(t),
	DataInputBase(t, first_row, first_col),
	DataOutputWithRegion(t, first_row + 3, first_col)
{
	set_fields(p, T, v, u, h, s, x);
}

void DataInputOutput::set_fields(DataEntryPair& in1, DataEntryPair& in2,
		DataEntryPair& out1, DataEntryPair& out2,
		DataEntryPair& out3, DataEntryPair& out4,
		DataEntryPair& out5)
{
	DataInputBase::set_fields(in1, in2, out1, out2, out3, out4, out5);
	DataOutputWithRegion::set_fields(in1, in2, out1, out2, out3, out4, out5);
}

LockedDataInputOutput::LockedDataInputOutput(Gtk::Table& t, int first_row,
		Function locked_func, double start_uval, int first_col)
	: DataIOBase(t),
	DataInputOutput(t, first_row - 1, first_col)
{
	_parent.remove(func_label);
	_parent.remove(func_chooser);

	set_function(locked_func);
	input_entry1->set_value(start_uval);
}

void LockedDataInputOutput::disable()
{
	region_label.set_text("Invalid input");

	p.set_sensitive(false);
	T.set_sensitive(false);
	v.set_sensitive(false);
	u.set_sensitive(false);
	h.set_sensitive(false);
	s.set_sensitive(false);
	x.set_sensitive(false);
}

void LockedDataInputOutput::set_user_value_range(double min, double max)
{
	input_entry1->set_range(min, max);

	// re-enable if necessary
	input_entry1->set_sensitive();
}

void LockedDataInputOutput::set_controlled_value(double val)
{
	set_value2(val);

	// re-enable if necessary
	input_entry2->set_sensitive();
}
