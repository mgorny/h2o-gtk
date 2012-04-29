/* h2o-gtk+ -- GTK+ UI to libh2o
 * (c) 2012 Michał Górny
 * Released under the terms of the 2-clause BSD license
 */

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif

#include "saturationbox.hxx"

HeatOfVaporizationEntry::HeatOfVaporizationEntry(double val)
{
	label.set_text_with_mnemonic("_r");
	set_tooltip_text("Specific heat of vaporization");
}

SaturationBox::SaturationBox()
	: Gtk::Table(8, 9),
	T(372.75), // @ 0.1 MPa
	prim(*this, 4, f_Tx, 473.15, 0, 1),
	bis(*this, 4, f_Tx, 473.15, 1, 5),
	prim_label("water (')"),
	bis_label("steam (\")")
{
	set_col_spacings(10);

	p.set_range(611.213E-6, 22.064);
	T.set_range(273.15, 647.096);

	p.add_to_table(*this, 0, 1);
	T.add_to_table(*this, 1, 1);

	r.add_to_table(*this, 1, 5);

	attach(sep, 0, 9, 2, 3);
	attach(vsep, 4, 5, 0, 10);

	attach(prim_label, 1, 4, 3, 4);
	attach(bis_label, 5, 8, 3, 4);

	p_conn = p.signal_value_changed().connect(
			sigc::mem_fun(*this, &SaturationBox::recalc_from_p));
	T.signal_value_changed().connect(
			sigc::mem_fun(*this, &SaturationBox::recalc));
}

void SaturationBox::recalc_from_p()
{
	double pval = p.get_value();
	double Tval = h2o::H2O::px(pval, 0).T();

	T.set_value(Tval);
}

void SaturationBox::recalc()
{
	double Tval = T.get_value();

	prim.set_value1(Tval);
	bis.set_value1(Tval);

	h2o::H2O medium[2];

	medium[0] = prim.get_h2o();
	medium[1] = bis.get_h2o();

	p_conn.block();
	p.set_value(medium[0].p());
	p_conn.unblock();
	r.set_readonly_value(medium[1].h() - medium[0].h());

	data_changed.emit(medium, 2);
}

SaturationBox::data_changed_sig SaturationBox::signal_data_changed()
{
	return data_changed;
}
