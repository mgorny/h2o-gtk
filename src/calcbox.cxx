/* h2o-gtk+ -- GTK+ UI to libh2o
 * (c) 2012 Michał Górny
 * Released under the terms of the 2-clause BSD license
 */

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif

#include "calcbox.hxx"

#include <stdexcept>

CalcBox::CalcBox()
	: Gtk::Table(9, 3),
	data_io(*this, 0)
{
	set_col_spacings(10);
}

void CalcBox::recalc()
{
	data_io.recalc();
}

CalcBox::data_changed_sig CalcBox::signal_data_changed()
{
	return data_io.signal_data_changed();
}
