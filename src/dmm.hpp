/*
 * Copyright (c) 2019 Analog Devices Inc.
 *
 * This file is part of Scopy
 * (see http://www.github.com/analogdevicesinc/scopy).
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef DMM_HPP
#define DMM_HPP

#include <QPushButton>
#include <QWidget>
#include <atomic>

#include "apiObject.hpp"
#include "filter.hpp"
#include "iio_manager.hpp"
#include "signal_sample.hpp"
#include "tool.hpp"
#include "scroll_filter.hpp"
#include <thread>
#include "spinbox_a.hpp"
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>
#include <boost/circular_buffer.hpp>

/* libm2k includes */
#include <libm2k/analog/m2kanalogin.hpp>
#include <libm2k/m2k.hpp>

namespace Ui {
	class DMM;
}

class QJSEngine;

namespace adiscope {
	class DMM_API;

	class DMM : public Tool
	{
		friend class DMM_API;
		friend class ToolLauncher_API;

		Q_OBJECT

	public:
		explicit DMM(struct iio_context *ctx, Filter *filt,
				ToolMenuItem *toolMenuItem, QJSEngine *engine,
				ToolLauncher *parent);
		~DMM();

	private:
		libm2k::context::M2k* m_m2k_context;
		libm2k::analog::M2kAnalogIn* m_m2k_analogin;
		unsigned int m_adc_nb_channels;
		Ui::DMM *ui;
		boost::shared_ptr<iio_manager> manager;
		iio_manager::port_id id_ch1, id_ch2;
		boost::shared_ptr<signal_sample> signal;
		unsigned long sample_rate;

		std::atomic<bool> interrupt_data_logging;
		std::atomic<bool> data_logging;
		QString filename;
		std::thread data_logging_thread;
		bool use_timer;
		unsigned long logging_refresh_rate;
		PositionSpinButton *data_logging_timer;

		boost::mutex data_mutex;
		boost::condition_variable data_cond;
		MouseWheelWidgetGuard *wheelEventGuard;

		std::vector<double> m_min, m_max;

		std::vector<bool> m_autoGainEnabled;
		std::vector<boost::circular_buffer<libm2k::analog::M2K_RANGE>> m_gainHistory;
		int m_gainHistorySize;

		void disconnectAll();
		gr::basic_block_sptr configureGraph(gr::basic_block_sptr s2f,
				bool is_ac);
		void configureModes();
		libm2k::analog::M2K_RANGE suggestRange(double volt_max, double volt_min);
		int numSamplesFromIdx(int idx);
		void writeAllSettingsToHardware();
		void checkPeakValues(int, double);
		bool isIioManagerStarted() const;
		void checkAndUpdateGainMode(const std::vector<double> &volts);

	public Q_SLOTS:
		void toggleTimer(bool start);
		void run() override;
		void stop() override;

	private Q_SLOTS:
		void setHistorySizeCh1(int idx);
		void setHistorySizeCh2(int idx);

		void setLineThicknessCh1(int idx);
                void setLineThicknessCh2(int idx);

                void updateValuesList(std::vector<float> values);

		void toggleAC();

		void enableDataLogging(bool);

		void toggleDataLogging(bool);

		void startDataLogging(bool);

		void dataLoggingThread();

		void chooseFile();

		void resetPeakHold(bool);

		void displayPeakHold(bool);

		void collapsePeakHold(bool);
		void collapseDataLog(bool);

		void readPreferences();

		void gainModeChanged(int idx);

	Q_SIGNALS:
		void showTool();
	};
}

#endif /* DMM_HPP */
