/* h2o-gtk+ -- GTK+ UI to libh2o
 * (c) 2012 Michał Górny
 * Released under the terms of the 2-clause BSD license
 */

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif

#include "saturationbox.hxx"

SaturationBox::SaturationBox()
	: Gtk::Table(2, 3),
	p("_p", "MPa", 611.213E-6, 22.064, 0.1, 5, 4, 1),
	T("_T", "K", 273.15, 647.096, 1, 50, 2)
{
	set_col_spacings(10);

	p.add_to_table(*this, 0);
	T.add_to_table(*this, 1);

	p.signal_value_changed().connect(sigc::mem_fun(*this, &SaturationBox::recalc));
	T.signal_value_changed().connect(sigc::mem_fun(*this, &SaturationBox::recalc_from_T));
}

void SaturationBox::recalc()
{
	h2o::H2O medium[2];

	medium[0] = h2o::H2O::px(p.get_value(), 0);
	medium[1] = h2o::H2O::px(p.get_value(), 1);

	T.set_value(medium[0].T());

	data_changed.emit(medium, 2);
}

void SaturationBox::recalc_from_T()
{
	h2o::H2O medium[2];

	medium[0] = h2o::H2O::Tx(T.get_value(), 0);
	medium[1] = h2o::H2O::Tx(T.get_value(), 1);

	p.set_value(medium[0].p());

	data_changed.emit(medium, 2);
}

SaturationBox::data_changed_sig SaturationBox::signal_data_changed()
{
	return data_changed;
}
