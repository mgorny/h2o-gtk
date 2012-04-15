/* h2o-gtk+ -- GTK+ UI to libh2o
 * (c) 2012 Michał Górny
 * Released under the terms of the 2-clause BSD license
 */

#pragma once

#ifndef _H2O_GTK_CALCBOX_HXX
#define _H2O_GTK_CALCBOX_HXX 1

#include "dataentrypair.hxx"

#include <h2o>

#include <sigc++/functors/mem_fun.h>
#include <sigc++/connection.h>
#include <sigc++/signal.h>
#include <gtkmm/comboboxtext.h>
#include <gtkmm/label.h>
#include <gtkmm/table.h>

class FunctionChoiceComboBox : public Gtk::ComboBoxText
{
public:
	FunctionChoiceComboBox();
};

class CalcBox : public Gtk::Table
{
	sigc::connection conn1, conn2;
	typedef sigc::signal<void, h2o::H2O*, int>
		data_changed_sig;

protected:
	FunctionChoiceComboBox func_chooser;
	DataEntryPair p, T, v, u, h, s, x;
	Gtk::Label func_label, region_label;
	data_changed_sig data_changed;

public:
	CalcBox();

	void remove_fields();
	void set_fields(DataEntryPair& in1, DataEntryPair& in2,
			DataEntryPair& out1, DataEntryPair& out2,
			DataEntryPair& out3, DataEntryPair& out4,
			DataEntryPair& out5);
	void recalc();
	void reorder_fields();

	data_changed_sig signal_data_changed();
};

#endif /*_H2O_GTK_CALCBOX_HXX*/
