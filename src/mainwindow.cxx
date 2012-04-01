/* h2o-gtk+ -- GTK+ UI to libh2o
 * (c) 2012 Michał Górny
 * Released under the terms of the 2-clause BSD license
 */

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif

#include "mainwindow.hxx"

#include <stdexcept>

#include <h2o>

DataEntryPair::DataEntryPair(const char* desc, const char* unit,
			double min, double max, double step, double pagestep,
			double decplaces, double val)
	: Gtk::SpinButton(0, decplaces),
	label(desc, Gtk::ALIGN_END, Gtk::ALIGN_CENTER),
	unit_label(unit, Gtk::ALIGN_START, Gtk::ALIGN_CENTER)
{
	get_adjustment()->configure(val, min, max, step, pagestep, 0);
}

DataEntryPair::DataEntryPair(const char* desc, const char* unit,
			double min, double max, double step, double pagestep,
			double decplaces)
	: Gtk::SpinButton(0, decplaces),
	label(desc, Gtk::ALIGN_END, Gtk::ALIGN_CENTER),
	unit_label(unit, Gtk::ALIGN_START, Gtk::ALIGN_CENTER)
{
	get_adjustment()->configure(0, min, max, step, pagestep, 0);

	set_editable(false);
}

DataEntryPair::~DataEntryPair()
{
}

void DataEntryPair::add_to_table(Gtk::Table& t, int row)
{
	t.attach(label, 0, 1, row, row + 1);
	t.attach(*this, 1, 2, row, row + 1);
	t.attach(unit_label, 2, 3, row, row + 1);
}

CalcBox::CalcBox()
	: Gtk::Table(7, 3),
	p("p", "MPa", 1E-4, 100, 0.1, 5, 4, 10),
	T("T", "K", 273.16, 2273.15, 1, 50, 2, 773.15),
	v("v", "m³/kg", 0, 1E17, 0.01, 1, 4),
	u("u", "kJ/kg", 0, 6400, 10, 200, 2),
	h("h", "kJ/kg", 0, 7500, 10, 200, 2),
	s("s", "kJ/kgK", 0, 28, 0.1, 2, 3)
{
	set_col_spacings(10);

	recalc();

	p.add_to_table(*this, 0);
	T.add_to_table(*this, 1);

	v.add_to_table(*this, 3);
	u.add_to_table(*this, 4);
	h.add_to_table(*this, 5);
	s.add_to_table(*this, 6);

	region_label.set_padding(0, 10);
	region_label.set_alignment(Gtk::ALIGN_START, Gtk::ALIGN_END);
	attach(region_label, 1, 3, 2, 3);

	p.signal_value_changed().connect(sigc::mem_fun(*this, &CalcBox::recalc));
	T.signal_value_changed().connect(sigc::mem_fun(*this, &CalcBox::recalc));
#if 0
	v.signal_value_changed().connect(sigc::mem_fun(*this, &CalcBox::recalc));
	u.signal_value_changed().connect(sigc::mem_fun(*this, &CalcBox::recalc));
	h.signal_value_changed().connect(sigc::mem_fun(*this, &CalcBox::recalc));
	s.signal_value_changed().connect(sigc::mem_fun(*this, &CalcBox::recalc));
#endif
}

CalcBox::~CalcBox()
{
}

void CalcBox::recalc()
{
	try
	{
		h2o::H2O medium(p.get_value(), T.get_value());
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

		v.set_value(medium.v());
		u.set_value(medium.u());
		h.set_value(medium.h());
		s.set_value(medium.s());
	}
	catch (std::range_error)
	{
		region_label.set_text("Parameters out-of-range");

		v.set_value(0);
		u.set_value(0);
		h.set_value(0);
		s.set_value(0);
	}
	catch (std::runtime_error)
	{
		region_label.set_text("Region not implemented");

		v.set_value(0);
		u.set_value(0);
		h.set_value(0);
		s.set_value(0);
	};
}

MainWindow::MainWindow()
{
	set_border_width(10);

	add(calcbox);

	show_all();
}

MainWindow::~MainWindow()
{
}
