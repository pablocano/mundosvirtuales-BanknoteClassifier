package com.mv.serverethernetip.utils.logger;

import java.io.PrintStream;
import java.text.SimpleDateFormat;
import java.util.Queue;
import java.util.concurrent.ConcurrentLinkedQueue;

import com.mv.serverethernetip.utils.ConsoleColors;

public class Logger {

	private static Logger INSTANCE;

	protected final Queue<ILoggerListener> listLogListener = new ConcurrentLinkedQueue<ILoggerListener>();

	private Logger() {

	}

	public static Logger getInstance() {

		if (INSTANCE == null) {

			INSTANCE = new Logger();

		}

		return INSTANCE;
	}

	public void addLoggerListener(ILoggerListener logListener) {

		listLogListener.add(logListener);

	}
	
	public void defaultPrintStream(LOGData dat, PrintStream ps) {

		final SimpleDateFormat format = new SimpleDateFormat("HH:mm:ss:SSS");

		String msg = dat.message;

		if (dat.errMessage != null) {

			msg += "\t" + dat.errMessage;

		}

		switch (dat.typeLog) {
		
		case INFO:

			break;

		case ERROR:
		case DEBUG:
		default:

		}

		ps.println(String.format("<%.7s>\t%s\t|%-20s|\t|%-20s|\t%s", dat.typeLog, format.format(dat.date),
				Thread.currentThread().getName(), dat.tag, msg));

	}

	public void defaultConsoleColor(LOGData dat, PrintStream ps) {

		final SimpleDateFormat format = new SimpleDateFormat("HH:mm:ss:SSS");

		String msg = dat.message;

		if (dat.errMessage != null) {

			msg += "\t" + dat.errMessage;

		}

		String ESC = "";

		switch (dat.typeLog) {

		case ERROR:

			ESC = ConsoleColors.RED_BACKGROUND + ConsoleColors.WHITE_BOLD_BRIGHT;

			break;

		case DEBUG:

			ESC = ConsoleColors.GREEN_BOLD;

			break;

		case INFO:

			ESC = ConsoleColors.BLACK_BOLD;

			break;

		default:

			ESC = ConsoleColors.BLACK;

		}

		ps.println(ESC + String.format("<%.7s>\t%s\t|%-20s|\t|%-20s|\t%s", dat.typeLog, format.format(dat.date),
				Thread.currentThread().getName(), dat.tag, msg) + ConsoleColors.RESET);

	}

	public void logDebug(String tag, String msg) {

		logPrint(ETypeLog.DEBUG, tag, msg, "");

	}

	public void logDebug(String tag, String msg, String err) {

		logPrint(ETypeLog.DEBUG, tag, msg, err);

	}

	public void logError(String tag, String msg) {

		logPrint(ETypeLog.ERROR, tag, msg, "");

	}

	public void logError(String tag, String msg, String err) {

		logPrint(ETypeLog.ERROR, tag, msg, err);

	}

	public void logInfo(String tag, String msg) {

		logPrint(ETypeLog.INFO, tag, msg, "");

	}

	public void logInfo(String tag, String msg, String err) {

		logPrint(ETypeLog.INFO, tag, msg, err);

	}

	protected void logPrint(ETypeLog type, String tag, String msg, String err) {

		LOGData dat = new LOGData(type, tag, msg, err);

		for (ILoggerListener logListener : listLogListener) {

			logListener.onLogEvent(dat);

		}

	}

}
