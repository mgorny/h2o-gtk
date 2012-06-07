/* h2o-gtk+ -- GTK+ UI to libh2o
 * (c) 2012 Michał Górny
 * Released under the terms of the 2-clause BSD license
 */

#pragma once

#ifndef _H2O_GTK_DATAENTRYPAIR_HXX
#define _H2O_GTK_DATAENTRYPAIR_HXX 1

#include <h2o>

#include <gtkmm/label.h>
#include <gtkmm/spinbutton.h>
#include <gtkmm/table.h>

class DataEntryPair : public Gtk::SpinButton
{
	double def_min, def_max;
	bool attached;

protected:
	Gtk::Label label;
	Gtk::Label unit_label;

public:
	DataEntryPair(const char* desc, const char* unit,
			double min, double max, double step, double pagestep,
			double decplaces, double val);

	void add_to_table(Gtk::Table& t, int row, int col = 0);
	void remove_from_table(Gtk::Table& t);

	void enable();
	void disable();

	void set_readonly_value(double newval);
};

class PressureEntry : public DataEntryPair
{
public:
	PressureEntry(double val = 0);
};

class TemperatureEntry : public DataEntryPair
{
public:
	TemperatureEntry(double val = 0);
};

class VolumeEntry : public DataEntryPair
{
public:
	VolumeEntry(double val = 0);
};

class InternalEnergyEntry : public DataEntryPair
{
public:
	InternalEnergyEntry(double val = 0);
};

class EnthalpyEntry : public DataEntryPair
{
public:
	EnthalpyEntry(double val = 0);
};

class EnthropyEntry : public DataEntryPair
{
public:
	EnthropyEntry(double val = 0);
};

class DrynessEntry : public DataEntryPair
{
public:
	DrynessEntry(double val = 0);
};

class DensityEntry : public DataEntryPair
{
public:
	DensityEntry(double val = 0);
};

class EfficiencyEntry : public DataEntryPair
{
public:
	EfficiencyEntry(double val = 0);
};

#endif /*_H2O_GTK_DATAENTRYPAIR_HXX*/
