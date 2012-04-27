/* h2o-gtk+ -- GTK+ UI to libh2o
 * (c) 2012 Michał Górny
 * Released under the terms of the 2-clause BSD license
 */

#pragma once

#ifndef _H2O_GTK_DATAINPUTOUTPUT_HXX
#define _H2O_GTK_DATAINPUTOUTPUT_HXX 1

#include "dataentrypair.hxx"

#include <h2o>

#include <sigc++/functors/mem_fun.h>
#include <sigc++/connection.h>
#include <sigc++/signal.h>
#include <gtkmm/comboboxtext.h>
#include <gtkmm/label.h>
#include <gtkmm/table.h>

enum Function
{
	f_pT = 0,
	f_ph,
	f_ps,
	f_px,
	f_Tx,
	f_hs,
	f_rhoT
};

class FunctionChoiceComboBox : public Gtk::ComboBoxText
{
public:
	FunctionChoiceComboBox();

	enum Function get_function();
};

class DataIOBase
{
	sigc::connection conn1, conn2;
	typedef sigc::signal<void, h2o::H2O*, int>
		data_changed_sig;

	void remove_fields();
	void reorder_fields();

protected:
	Gtk::Table& _parent;
	int _first_row, _first_col;

	data_changed_sig data_changed;
	FunctionChoiceComboBox func_chooser;
	DataEntryPair p, T, v, u, h, s, x, rho;
	Gtk::Label func_label;

	virtual void set_fields(DataEntryPair& in1, DataEntryPair& in2,
			DataEntryPair& out1, DataEntryPair& out2,
			DataEntryPair& out3, DataEntryPair& out4,
			DataEntryPair& out5);

	DataIOBase(Gtk::Table& t, int first_row, int first_col);

public:
	h2o::H2O get_h2o();
	void recalc();

	data_changed_sig signal_data_changed();
};

class DataInputBase : virtual public DataIOBase
{
protected:
	virtual void set_fields(DataEntryPair& in1, DataEntryPair& in2,
			DataEntryPair& out1, DataEntryPair& out2,
			DataEntryPair& out3, DataEntryPair& out4,
			DataEntryPair& out5);

	DataInputBase(Gtk::Table& t, int first_row, int first_col = 0);
};

class DataInput : public DataInputBase
{
public:
	DataInput(Gtk::Table& t, int first_row, int first_col = 0);
};

class DataOutputBase : virtual public DataIOBase
{
	void recalc_for(h2o::H2O* data, int len);

protected:
	virtual void set_fields(DataEntryPair& in1, DataEntryPair& in2,
			DataEntryPair& out1, DataEntryPair& out2,
			DataEntryPair& out3, DataEntryPair& out4,
			DataEntryPair& out5);

	DataOutputBase(Gtk::Table& t, int first_row, int first_col = 0);
};

class DataOutput : public DataOutputBase
{
protected:
	DataEntryPair* input_entry1;
	DataEntryPair* input_entry2;

	virtual void set_fields(DataEntryPair& in1, DataEntryPair& in2,
			DataEntryPair& out1, DataEntryPair& out2,
			DataEntryPair& out3, DataEntryPair& out4,
			DataEntryPair& out5);

public:
	DataOutput(Gtk::Table& t, int first_row,
			Function locked_func,
			double start_val1 = 0, double start_val2 = 0,
			int first_col = 0);

	void set_function(Function f);
	void set_value1(double val);
	void set_value2(double val);
};

class DataOutputWithRegion : public DataOutputBase
{
	void recalc_for(h2o::H2O* data, int len);

protected:
	Gtk::Label region_label;

	virtual void set_fields(DataEntryPair& in1, DataEntryPair& in2,
			DataEntryPair& out1, DataEntryPair& out2,
			DataEntryPair& out3, DataEntryPair& out4,
			DataEntryPair& out5);

	DataOutputWithRegion(Gtk::Table& t, int first_row, int first_col = 0);
};

class DataInputOutput : public DataInputBase, public DataOutputWithRegion
{
protected:
	virtual void set_fields(DataEntryPair& in1, DataEntryPair& in2,
			DataEntryPair& out1, DataEntryPair& out2,
			DataEntryPair& out3, DataEntryPair& out4,
			DataEntryPair& out5);

public:
	DataInputOutput(Gtk::Table& t, int first_row, int first_col = 0);
};

class LockedDataInputOutput : public DataInputOutput
{
	DataEntryPair* controlled_entry;
	DataEntryPair* user_entry;

protected:
	virtual void set_fields(DataEntryPair& in1, DataEntryPair& in2,
			DataEntryPair& out1, DataEntryPair& out2,
			DataEntryPair& out3, DataEntryPair& out4,
			DataEntryPair& out5);

public:
	LockedDataInputOutput(Gtk::Table& t, int first_row,
			Function locked_func, double start_uval = 10,
			int first_col = 0);

	void set_user_value_range(double min, double max);
	void set_controlled_value(double val);
};

#endif /*_H2O_GTK_DATAINPUTOUTPUT_HXX*/
