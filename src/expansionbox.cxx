/* h2o-gtk+ -- GTK+ UI to libh2o
 * (c) 2012 Michał Górny
 * Released under the terms of the 2-clause BSD license
 */

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif

#include "expansionbox.hxx"

#include <cassert>
#include <stdexcept>

RealExpansionInputOutput::RealExpansionInputOutput(
		Gtk::Table& t, int first_row,
		double start_eta, int first_col)
	: DataIOBase(t),
	LockedDataInputOutput(t, first_row, f_ph, 0, first_col),
	eta("\316\267", "", 0, 1, 0.001, 0.02, 3, start_eta)
{
	p.remove_from_table(_parent);
	eta.add_to_table(_parent,
			DataInputBase::_first_row + 1, DataInputBase::_first_col);

	eta.signal_value_changed().connect(
			sigc::mem_fun(*this, &RealExpansionInputOutput::recalc));
}

void RealExpansionInputOutput::recalc()
{
	double hout = _hin - (_hin - _hout) * eta.get_value();

	h.set_readonly_value(hout);
}

void RealExpansionInputOutput::set_hin_hout(double hin, double hout)
{
	_hin = hin;
	_hout = hout;
	recalc();
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
}

void ExpansionBox::input_changed(h2o::H2O* data, int len)
{
	assert(len == 1);

	cached_input = data[0];

	out_io.set_user_value_range(1E-3, cached_input.p());
	out_io.set_controlled_value(cached_input.s());
}

void ExpansionBox::output_changed(h2o::H2O* data, int len)
{
	assert(len == 1);

	real_io.set_value1(data[0].p());
	real_io.set_hin_hout(cached_input.h(), data[0].h());

	h2o::H2O plot_data[2];

	plot_data[0] = data[0];
	plot_data[1] = cached_input;

	data_changed.emit(plot_data, 2);
}

void ExpansionBox::recalc()
{
	in_io.recalc();
}

ExpansionBox::data_changed_sig ExpansionBox::signal_data_changed()
{
	return data_changed;
}
