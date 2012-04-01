/* h2o-gtk+ -- GTK+ UI to libh2o
 * (c) 2012 Michał Górny
 * Released under the terms of the 2-clause BSD license
 */

#pragma once

#ifndef _H2O_GTK_MAINWINDOW_HXX
#define _H2O_GTK_MAINWINDOW_HXX 1

#include <sigc++/functors/mem_fun.h>
#include <gtkmm/label.h>
#include <gtkmm/spinbutton.h>
#include <gtkmm/table.h>
#include <gtkmm/window.h>

class DataEntryPair : public Gtk::SpinButton
{
protected:
	Gtk::Label label;
	Gtk::Label unit_label;

public:
	DataEntryPair(const char* desc, const char* unit,
			double min, double max, double step, double pagestep,
			double decplaces, double val);
	DataEntryPair(const char* desc, const char* unit,
			double min, double max, double step, double pagestep,
			double decplaces);
	virtual ~DataEntryPair();

	void add_to_table(Gtk::Table& t, int row);
};

class CalcBox : public Gtk::Table
{
protected:
	DataEntryPair p, T, v, u, h, s;
	Gtk::Label region_label;

public:
	CalcBox();
	virtual ~CalcBox();

	void recalc();
};

class MainWindow : public Gtk::Window
{
protected:
	CalcBox calcbox;

public:
	MainWindow();
	virtual ~MainWindow();
};

#endif /*_H2O_GTK_MAINWINDOW_HXX*/
