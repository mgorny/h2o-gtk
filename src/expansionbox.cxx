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

ExpansionWorkEntry::ExpansionWorkEntry(double val)
{
	label.set_text_with_mnemonic("_w");
	set_tooltip_text("Specific real expansion work");
}

ExpansionHeatLossEntry::ExpansionHeatLossEntry(double val)
{
	label.set_text_with_mnemonic("\316\224_h");
	set_tooltip_text("Specific real expansion heat loss");
}

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

double RealExpansionInputOutput::get_eta() const
{
	return eta.get_value();
}

void RealExpansionInputOutput::recalc(h2o::H2O& in, h2o::H2O& out)
{
	h2o::H2O exp;

	try
	{
		if (!in.initialized() || !out.initialized())
			throw std::range_error("");

		exp = in.expand(out.p(), eta.get_value());

		p.set_readonly_value(exp.p());
		h.set_readonly_value(exp.h());
		eta.set_sensitive();
	}
	catch (std::range_error)
	{
		h.set_readonly_value(0);
		disable();
	}

	LockedDataInputOutput::recalc();
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

	w.add_to_table(*this, 1, 5);
	dh.add_to_table(*this, 2, 5);
	w.disable();
	dh.disable();

	in_io.signal_data_changed().connect(
			sigc::mem_fun(*this, &ExpansionBox::input_changed));
	out_io.signal_data_changed().connect(
			sigc::mem_fun(*this, &ExpansionBox::output_changed));
	real_io.signal_data_changed().connect(
			sigc::mem_fun(*this, &ExpansionBox::real_changed));

	real_io.signal_eta_changed().connect(
			sigc::mem_fun(*this, &ExpansionBox::recalc));

	in_io.recalc();
}

void ExpansionBox::input_changed(h2o::H2O* data, int len)
{
	assert(len == 1);

	cached_data[1] = data[0];

	if (data[0].initialized())
	{
		out_io.set_user_value_range(1E-3, data[0].p());
		out_io.set_controlled_value(data[0].s());
	}
	else
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
	recalc();
}

void ExpansionBox::real_changed(h2o::H2O* data, int len)
{
	assert(len == 1);

	const int stepshi = 5;
	const int stepslo = 15;
	const int plot_len = stepshi + stepslo + 3;
	h2o::H2O plot_data[plot_len];

	plot_data[0] = cached_data[0];
	plot_data[1] = cached_data[1];
	plot_data[plot_len - 1] = data[0];

	if (cached_data[0].initialized() && cached_data[1].initialized()
			&& data[0].initialized())
	{
		const double p0 = cached_data[1].p();
		const double p1 = data[0].p();
		const double prange = (p1 - p0) / 6; // should be enough
		const double psteplo = prange / stepslo;
		const double pstephi = (p1 - p0 - prange) / stepshi;

		const double eta = real_io.get_eta();

		int i;
		for (i = 1; i <= stepslo; ++i)
			plot_data[plot_len - i - 1] = cached_data[1].expand(
					p1 - psteplo * i, eta);
		for (i = 1; i <= stepshi; ++i)
			plot_data[2 + i] = cached_data[1].expand(
					p0 + pstephi * i, eta);

		w.set_readonly_value(cached_data[1].h() - data[0].h());
		dh.set_readonly_value(data[0].h() - cached_data[0].h());

		w.set_sensitive();
		dh.set_sensitive();
	}
	else
	{
		w.set_sensitive(false);
		dh.set_sensitive(false);
	}

	data_changed.emit(plot_data, plot_len);
}

void ExpansionBox::recalc()
{
	real_io.recalc(cached_data[1], cached_data[0]);
}

ExpansionBox::data_changed_sig ExpansionBox::signal_data_changed()
{
	return data_changed;
}
