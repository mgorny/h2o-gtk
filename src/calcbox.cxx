/* h2o-gtk+ -- GTK+ UI to libh2o
 * (c) 2012 Michał Górny
 * Released under the terms of the 2-clause BSD license
 */

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif

#include "calcbox.hxx"

#include <stdexcept>

enum functions
{
	f_pT = 0,
	f_ph,
	f_ps,
	f_px,
	f_Tx
};

FunctionChoiceComboBox::FunctionChoiceComboBox()
{
	append("f(p, T)");
	append("f(p, h)");
	append("f(p, s)");
	append("f(p, x)");
	append("f(T, x)");

	set_active(0);
}

DataEntryPair::DataEntryPair(const char* desc, const char* unit,
			double min, double max, double step, double pagestep,
			double decplaces, double val)
	: Gtk::SpinButton(0, decplaces),
	def_min(min), def_max(max),
	label(desc, Gtk::ALIGN_END, Gtk::ALIGN_CENTER),
	unit_label(unit, Gtk::ALIGN_START, Gtk::ALIGN_CENTER)
{
	get_adjustment()->configure(val, min, max, step, pagestep, 0);
}

void DataEntryPair::add_to_table(Gtk::Table& t, int row)
{
	t.attach(label, 0, 1, row, row + 1);
	t.attach(*this, 1, 2, row, row + 1);
	t.attach(unit_label, 2, 3, row, row + 1);
}

void DataEntryPair::remove_from_table(Gtk::Table& t)
{
	t.remove(label);
	t.remove(*this);
	t.remove(unit_label);
}

void DataEntryPair::enable()
{
	set_editable(true);
	set_range(def_min, def_max);
}

void DataEntryPair::disable()
{
	double val = get_value();

	set_editable(false);
	set_range(val, val);
}

void DataEntryPair::set_value(double val)
{
	set_range(val, val);
#if 0 // set_range() should be enough
	Gtk::SpinButton::set_value(val);
#endif
}

CalcBox::CalcBox()
	: Gtk::Table(9, 3),
	p("p", "MPa", 1E-4, 100, 0.1, 5, 4, 10),
	T("T", "K", 273.15, 2273.15, 1, 50, 2, 773.15),
	v("v", "m³/kg", 0, 1E17, 0.01, 1, 6),
	u("u", "kJ/kg", 0, 6400, 10, 200, 2),
	h("h", "kJ/kg", 0, 7500, 10, 200, 2),
	s("s", "kJ/kgK", 0, 28, 0.04, 0.2, 3),
	x("x", "[-]", 0, 1, 0.004, 0.02, 3)
{
	set_col_spacings(10);

	attach(func_chooser, 1, 2, 0, 1);

	set_fields(p, T, v, u, h, s, x);

	region_label.set_padding(0, 10);
	region_label.set_alignment(Gtk::ALIGN_START, Gtk::ALIGN_END);
	attach(region_label, 1, 3, 3, 4);

	func_chooser.signal_changed().connect(sigc::mem_fun(*this, &CalcBox::reorder_fields));
}

void CalcBox::remove_fields()
{
	conn1.disconnect();
	conn2.disconnect();

	p.remove_from_table(*this);
	T.remove_from_table(*this);

	v.remove_from_table(*this);
	u.remove_from_table(*this);
	h.remove_from_table(*this);
	s.remove_from_table(*this);
	x.remove_from_table(*this);
}

void CalcBox::set_fields(DataEntryPair& in1, DataEntryPair& in2,
		DataEntryPair& out1, DataEntryPair& out2,
		DataEntryPair& out3, DataEntryPair& out4,
		DataEntryPair& out5)
{
	in1.add_to_table(*this, 1);
	in2.add_to_table(*this, 2);
	in1.enable();
	in2.enable();

	conn1 = in1.signal_value_changed().connect(sigc::mem_fun(*this, &CalcBox::recalc));
	conn2 = in2.signal_value_changed().connect(sigc::mem_fun(*this, &CalcBox::recalc));

	out1.add_to_table(*this, 4);
	out2.add_to_table(*this, 5);
	out3.add_to_table(*this, 6);
	out4.add_to_table(*this, 7);
	out5.add_to_table(*this, 8);
	out1.disable();
	out2.disable();
	out3.disable();
	out4.disable();
	out5.disable();
}

#include <iostream>

void CalcBox::recalc()
{
	h2o::H2O medium;

	try
	{
		switch (func_chooser.get_active_row_number())
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
		}
	}
	catch (std::range_error)
	{
		region_label.set_text("Parameters out-of-range");
		return;
	}
	catch (std::runtime_error)
	{
		region_label.set_text("Region not implemented");
		return;
	};

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
			label_text = "Unknown/invalid";
	}
	region_label.set_text(label_text);

	switch (func_chooser.get_active_row_number())
	{
		case f_pT:
			h.set_value(medium.h());
			s.set_value(medium.s());
			x.set_value(medium.x());
			break;
		case f_ph:
			T.set_value(medium.T());
			s.set_value(medium.s());
			x.set_value(medium.x());
			break;
		case f_ps:
			T.set_value(medium.T());
			h.set_value(medium.h());
			x.set_value(medium.x());
			break;
		case f_px:
			T.set_value(medium.T());
			h.set_value(medium.h());
			s.set_value(medium.s());
			break;
		case f_Tx:
			p.set_value(medium.p());
			h.set_value(medium.h());
			s.set_value(medium.s());
			break;
	}

	v.set_value(medium.v());
	u.set_value(medium.u());

	data_changed.emit(&medium, 1);
}

void CalcBox::reorder_fields()
{
	remove_fields();

	switch (func_chooser.get_active_row_number())
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
	}

	recalc();
}

CalcBox::data_changed_sig CalcBox::signal_data_changed()
{
	return data_changed;
}
