/* h2o-gtk+ -- GTK+ UI to libh2o
 * (c) 2012 Michał Górny
 * Released under the terms of the 2-clause BSD license
 */

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif

#include "expansionbox.hxx"

#include <sigc++/functors/mem_fun.h>

#include <cassert>
#include <stdexcept>

RealExpansionInputOutput::RealExpansionInputOutput(
		Gtk::Table& t, int first_row,
		double start_eta, int first_col)
	: DataIOBase(t),
	LockedDataInputOutput(t, first_row, f_ph, 0, first_col),
	eta(start_eta)
{
	p.remove_from_table(_parent);
	eta.add_to_table(_parent,
			DataInputBase::_first_row + 1, DataInputBase::_first_col);

	eta.signal_value_changed().connect(
			sigc::mem_fun(*this,
				&RealExpansionInputOutput::eta_change_handler));
}

void RealExpansionInputOutput::recalc(h2o::H2O& in, h2o::H2O& out)
{
	h2o::H2O exp;

	try
	{
		exp = in.expand(out.p(), eta.get_value());

		p.set_readonly_value(exp, &h2o::H2O::p);
		h.set_readonly_value(exp, &h2o::H2O::h);
		eta.set_sensitive();
	}
	catch (std::runtime_error)
	{
		h.set_readonly_value(0);
		disable();
	}
}

void RealExpansionInputOutput::disable()
{
	LockedDataInputOutput::disable();

	eta.set_sensitive(false);
}

void RealExpansionInputOutput::eta_change_handler()
{
	eta_changed.emit();
}

RealExpansionInputOutput::eta_changed_sig
	RealExpansionInputOutput::signal_eta_changed()
{
	return eta_changed;
}

ExpansionBox::ExpansionBox()
	: Gtk::Table(13, 9),
	in_io(*this, 0, 1),
	out_io(*this, 5, f_ps, 0.1, 1),
	real_io(*this, 5, 0.9, 5)
{
	set_col_spacings(10);

	attach(sep, 0, 9, 4, 5);
	attach(vsep, 4, 5, 0, 13);

	in_io.signal_data_changed().connect(
			sigc::mem_fun(*this, &ExpansionBox::input_changed));
	out_io.signal_data_changed().connect(
			sigc::mem_fun(*this, &ExpansionBox::output_changed));
	real_io.signal_data_changed().connect(
			sigc::mem_fun(*this, &ExpansionBox::real_changed));

	real_io.signal_eta_changed().connect(
			sigc::mem_fun(*this, &ExpansionBox::eta_changed));
}

void ExpansionBox::input_changed(h2o::H2O* data, int len)
{
	assert(len == 1);

	cached_data[1] = data[0];

	try
	{
		out_io.set_user_value_range(1E-3, data[0].p());
		out_io.set_controlled_value(data[0].s());
	}
	catch (std::runtime_error)
	{
		// this is a hack to ensure re-calculation
		// after correcting input and re-enabling entries
		out_io.set_controlled_value(0);
		real_io.set_controlled_value(0);

		out_io.disable();
		real_io.disable();
	}
}

void ExpansionBox::output_changed(h2o::H2O* data, int len)
{
	assert(len == 1);

	cached_data[0] = data[0];

	// update real_io
	eta_changed();
}

void ExpansionBox::real_changed(h2o::H2O* data, int len)
{
	assert(len == 1);

	h2o::H2O plot_data[3];

	plot_data[0] = cached_data[0];
	plot_data[1] = cached_data[1];
	plot_data[2] = data[0];

	data_changed.emit(plot_data, 3);
}

void ExpansionBox::eta_changed()
{
	real_io.recalc(cached_data[1], cached_data[0]);
}

void ExpansionBox::recalc()
{
	in_io.recalc();
}

ExpansionBox::data_changed_sig ExpansionBox::signal_data_changed()
{
	return data_changed;
}
