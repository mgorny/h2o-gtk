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

ExpansionBox::ExpansionBox()
	: Gtk::Table(12, 3),
	in_io(*this, 0),
	out_io(*this, 5, f_ps, 0.1)
{
	set_col_spacings(10);

	attach(sep, 0, 3, 4, 5);

	in_io.signal_data_changed().connect(
			sigc::mem_fun(*this, &ExpansionBox::input_changed));
	out_io.signal_data_changed().connect(
			sigc::mem_fun(*this, &ExpansionBox::output_changed));
}

void ExpansionBox::input_changed(h2o::H2O* data, int len)
{
	assert(len == 1);

	cached_input = data[0];

	out_io.set_controlled_value(cached_input.s());
}

void ExpansionBox::output_changed(h2o::H2O* data, int len)
{
	assert(len == 1);

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
