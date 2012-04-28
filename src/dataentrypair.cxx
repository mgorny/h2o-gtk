/* h2o-gtk+ -- GTK+ UI to libh2o
 * (c) 2012 Michał Górny
 * Released under the terms of the 2-clause BSD license
 */

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif

#include <cassert>

#include "dataentrypair.hxx"

DataEntryPair::DataEntryPair(const char* desc, const char* unit,
			double min, double max, double step, double pagestep,
			double decplaces, double val)
	: Gtk::SpinButton(0, decplaces),
	def_min(min), def_max(max),
	attached(false),
	label(desc, Gtk::ALIGN_END, Gtk::ALIGN_CENTER, true),
	unit_label(unit, Gtk::ALIGN_START, Gtk::ALIGN_CENTER)
{
	get_adjustment()->configure(val, min, max, step, pagestep, 0);

	label.set_mnemonic_widget(*this);

	show();
	label.show();
	unit_label.show();
}

void DataEntryPair::add_to_table(Gtk::Table& t, int row, int col)
{
	assert(!attached);

	t.attach(label, col, col + 1, row, row + 1);
	t.attach(*this, col + 1, col + 2, row, row + 1);
	t.attach(unit_label, col + 2, col + 3, row, row + 1);

	attached = true;
}

void DataEntryPair::remove_from_table(Gtk::Table& t)
{
	if (attached)
	{
		t.remove(label);
		t.remove(*this);
		t.remove(unit_label);

		attached = false;
	}
}

void DataEntryPair::enable()
{
	set_editable(true);
	set_sensitive(true);
	set_range(def_min, def_max);
}

void DataEntryPair::disable()
{
	double val = get_value();

	set_editable(false);
	set_range(val, val);
}

void DataEntryPair::set_readonly_value(double val)
{
	set_range(val, val);
#if 0 // set_range() should be enough
	Gtk::SpinButton::set_value(val);
#endif
}

void DataEntryPair::set_readonly_value(const h2o::H2O& medium, MediumProperty prop)
{
	try
	{
		double v = (medium.*prop)();
		set_readonly_value(v);
		set_sensitive(true);
	}
	catch (std::runtime_error)
	{
		set_sensitive(false);
	};
}

PressureEntry::PressureEntry(double val)
	: DataEntryPair("_p", "MPa", 1E-4, 100, 0.1, 5, 4, val)
{
	set_tooltip_text("Pressure");
}

TemperatureEntry::TemperatureEntry(double val)
	: DataEntryPair("_T", "K", 273.15, 2273.15, 1, 50, 2, val)
{
	set_tooltip_text("Temperature");
}

VolumeEntry::VolumeEntry(double val)
	: DataEntryPair("_v", "m³/kg", 0, 1E17, 0.01, 1, 6, val)
{
	set_tooltip_text("Specific volume");
}

InternalEnergyEntry::InternalEnergyEntry(double val)
	: DataEntryPair("_u", "kJ/kg", 0, 6400, 10, 200, 2, val)
{
	set_tooltip_text("Specific internal energy");
}

EnthalpyEntry::EnthalpyEntry(double val)
	: DataEntryPair("_h", "kJ/kg", 0, 7500, 10, 200, 2, val)
{
	set_tooltip_text("Specific enthalpy");
}

EnthropyEntry::EnthropyEntry(double val)
	: DataEntryPair("_s", "kJ/kgK", 0, 28, 0.04, 0.2, 3, val)
{
	set_tooltip_text("Enthropy");
}

DrynessEntry::DrynessEntry(double val)
	: DataEntryPair("_x", "[-]", 0, 1, 0.004, 0.02, 3, val)
{
	set_tooltip_text("Dryness");
}

DensityEntry::DensityEntry(double val)
	: DataEntryPair("\317\201", "kg/m\302\263", 0, 1050, 5, 40, 2, val)
{
	set_tooltip_text("Density");
}

EfficiencyEntry::EfficiencyEntry(double val)
	: DataEntryPair("\316\267", "[-]", 0, 1, 0.001, 0.02, 3, val)
{
	set_tooltip_text("Efficiency");
}

HeatOfVaporizationEntry::HeatOfVaporizationEntry(double val)
{
	label.set_text_with_mnemonic("_r");
	set_tooltip_text("Specific heat of vaporization");
}
