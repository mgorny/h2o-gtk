/* h2o-gtk+ -- GTK+ UI to libh2o
 * (c) 2012 Michał Górny
 * Released under the terms of the 2-clause BSD license
 */

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif

#include "saturationbox.hxx"

SaturationBox::SaturationBox()
	: Gtk::Table(8, 7),
	p("_p", "MPa", 611.213E-6, 22.064, 0.1, 5, 4, 1),
	T("_T", "K", 273.15, 647.096, 1, 50, 2),
	r("_r", "kJ/kg", 0, 7500, 10, 200, 2),
	prim(*this, 4, f_Tx, 473.15, 0, 0),
	bis(*this, 4, f_Tx, 473.15, 1, 4),
	prim_label("water (')"),
	bis_label("steam (\")")
{
	set_col_spacings(10);

	p.add_to_table(*this, 0);
	T.add_to_table(*this, 1);

	r.add_to_table(*this, 1, 4);

	attach(sep, 0, 7, 2, 3);
	attach(vsep, 3, 4, 0, 8);

	attach(prim_label, 0, 3, 3, 4);
	attach(bis_label, 4, 7, 3, 4);

	p.signal_value_changed().connect(sigc::mem_fun(*this, &SaturationBox::recalc));
	T.signal_value_changed().connect(sigc::mem_fun(*this, &SaturationBox::recalc_from_T));
}

void SaturationBox::recalc()
{
	double pval = p.get_value();
	double Tval = h2o::H2O::px(pval, 0).T();

	T.set_value(Tval);
}

void SaturationBox::recalc_from_T()
{
	double Tval = T.get_value();

	prim.set_value1(Tval);
	bis.set_value1(Tval);

	h2o::H2O medium[2];

	medium[0] = prim.get_h2o();
	medium[1] = bis.get_h2o();

	p.set_value(medium[0].p());
	r.set_readonly_value(medium[1].h() - medium[0].h());

	data_changed.emit(medium, 2);
}

SaturationBox::data_changed_sig SaturationBox::signal_data_changed()
{
	return data_changed;
}
