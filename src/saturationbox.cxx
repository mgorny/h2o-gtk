/* h2o-gtk+ -- GTK+ UI to libh2o
 * (c) 2012 Michał Górny
 * Released under the terms of the 2-clause BSD license
 */

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif

#include "saturationbox.hxx"

SaturationBox::SaturationBox()
	: Gtk::Table(8, 3),
	p("_p", "MPa", 611.213E-6, 22.064, 0.1, 5, 4, 1),
	T("_T", "K", 273.15, 647.096, 1, 50, 2),
	h1("_h'", "kJ/kg", 0, 7500, 10, 200, 2),
	h2("_h\"", "kJ/kg", 0, 7500, 10, 200, 2),
	r("_r", "kJ/kg", 0, 7500, 10, 200, 2),
	s1("_s'", "kJ/kgK", 0, 28, 0.04, 0.2, 3),
	s2("_s\"", "kJ/kgK", 0, 28, 0.04, 0.2, 3)
{
	set_col_spacings(10);

	p.add_to_table(*this, 0);
	T.add_to_table(*this, 1);

	h1.add_to_table(*this, 3);
	h2.add_to_table(*this, 4);
	r.add_to_table(*this, 5);
	s1.add_to_table(*this, 6);
	s2.add_to_table(*this, 7);

	p.signal_value_changed().connect(sigc::mem_fun(*this, &SaturationBox::recalc));
	T.signal_value_changed().connect(sigc::mem_fun(*this, &SaturationBox::recalc_from_T));
}

void SaturationBox::fill_hs(h2o::H2O& water, h2o::H2O& steam)
{
	double h1v = water.h();
	double h2v = steam.h();

	h1.set_readonly_value(h1v);
	h2.set_readonly_value(h2v);
	r.set_readonly_value(h2v - h1v);

	s1.set_readonly_value(water.s());
	s2.set_readonly_value(steam.s());
}

void SaturationBox::recalc()
{
	h2o::H2O medium[2];

	medium[0] = h2o::H2O::px(p.get_value(), 0);
	medium[1] = h2o::H2O::px(p.get_value(), 1);

	T.set_value(medium[0].T());
	fill_hs(medium[0], medium[1]);

	data_changed.emit(medium, 2);
}

void SaturationBox::recalc_from_T()
{
	h2o::H2O medium[2];

	medium[0] = h2o::H2O::Tx(T.get_value(), 0);
	medium[1] = h2o::H2O::Tx(T.get_value(), 1);

	p.set_value(medium[0].p());
	fill_hs(medium[0], medium[1]);

	data_changed.emit(medium, 2);
}

SaturationBox::data_changed_sig SaturationBox::signal_data_changed()
{
	return data_changed;
}
