package com.mv.serverethernetip.interfaces;

import java.io.PrintStream;
import java.util.Scanner;
import java.util.concurrent.Semaphore;
import java.util.function.BiConsumer;

import com.mv.serverethernetip.server.ServerAbstract;
import com.mv.serverethernetip.utils.ExceptionGeneric;
import com.mv.serverethernetip.utils.logger.ETypeLog;
import com.mv.serverethernetip.utils.logger.ILoggerListener;
import com.mv.serverethernetip.utils.logger.LOGData;

public class InterfaceServerFile extends InterfaceServerAbstract {

	private enum TypePrint {

		INFO, INPUT, NONE, OUTPUT

	}
	private final static String PROMPT = "Server> ";

	private TypePrint lastPrint = TypePrint.NONE;

	protected final PrintStream printStream;

	private Semaphore semaphorePrint = new Semaphore(1, true); // TODO: change for lock and synchronized

	public InterfaceServerFile(String name, ServerAbstract server, PrintStream ps, BiConsumer<LOGData, PrintStream> fnConsumer) {

		super(name, server);

		printStream = ps;

		logger.addLoggerListener(new ILoggerListener() {

			@Override
			public void onLogEvent(LOGData dat) {

				if ((logDebug && (dat.typeLog == ETypeLog.DEBUG)) || (logError && (dat.typeLog == ETypeLog.ERROR))
						|| (logInfo && (dat.typeLog == ETypeLog.INFO))) {

					fnConsumer.accept(dat, printStream);

				}

			}
		});

	}

	@Override
	public void execute() {

		Scanner scanner = new Scanner(System.in);
		String command;

		do {

			command = readCommand(scanner);

			if (!command.toLowerCase().equals("quit")) {

				try {

					String result = server.processCommand(command);

					if (!result.isEmpty()) {

						print(System.out, result, TypePrint.OUTPUT);

					}

				} catch (ExceptionGeneric e) {

					logger.logError(name, "Error process command " + command, e.getMessage());

				}

			} else {

				break;

			}

		} while (true);

		scanner.close();

		server.close();

	}

	@Override
	protected void finalize() throws Throwable {

		printStream.close();

		super.finalize();
	}

	public void print(PrintStream ps, String msg) {

		print(ps, msg, TypePrint.INFO);

	}

	public void print(PrintStream ps, String msg, TypePrint in) {

		try {

			semaphorePrint.acquire();

			switch (lastPrint) {

			case INPUT:

				if (in == TypePrint.INFO) {

					ps.println("");

				}

				break;

			case INFO:

				ps.println("");

				break;

			case NONE:
			default:

				break;

			}

			if (in != TypePrint.OUTPUT) {

				ps.print(PROMPT + msg);

			} else {

				ps.println(msg);

			}

		} catch (InterruptedException e) {

			e.printStackTrace();

		} finally {

			lastPrint = in;

			semaphorePrint.release();

		}

	}

	public String processCommand(String command) {

		String result = "";

		String[] cmds = command.split("[, ]");

		switch (cmds[0].toLowerCase()) {

		case "debug":

		{
			if (cmds.length == 2) {

				result = "debug is ";

				String option = cmds[1].trim().toLowerCase();

				if (option.equals("on")) {

					this.logDebug = true;
					result += "on";

				} else if (option.equals("off")) {

					this.logDebug = false;
					result += "off";

				} else {

					result += "command is: debug < on | off > ";

				}

			}

		}

			break;

		case "error":

		{
			if (cmds.length == 2) {

				result = "error log is ";

				String option = cmds[1].trim().toLowerCase();

				if (option.equals("on")) {

					this.logError = true;
					result += "on";

				} else if (option.equals("off")) {

					this.logError = false;
					result += "off";

				} else {

					result += "command is: error < on | off > ";

				}

			}

		}

			break;

		default:

		}

		return result;
	}

	public String readCommand(Scanner scanner) {

		print(System.out, "", TypePrint.INPUT);
		return scanner.nextLine();

	}

}
