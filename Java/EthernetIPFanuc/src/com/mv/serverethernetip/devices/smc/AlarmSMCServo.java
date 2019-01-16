package com.mv.serverethernetip.devices.smc;

import java.util.HashMap;
import java.util.Map;

import com.mv.serverethernetip.devices.AlarmDevice;

public class AlarmSMCServo extends AlarmDevice {
	
	private final int N_ALARMS = 4;
	
	private final int[] alarms = new int[N_ALARMS];

	private static final Map<Integer, String> dicAlarms;
	static {
		dicAlarms = new HashMap<Integer, String>();

		// those alarms are deactivated with RESET
		dicAlarms.put(48, "Drive data value is wrong");
		dicAlarms.put(49, "Parameter value is wrong");
		dicAlarms.put(51, "Set step data is not registered on list");
		dicAlarms.put(52, "Set stroke is outside the stroke limit");
		dicAlarms.put(58, "Parameter is outside of settable range");
		dicAlarms.put(61, "Movement specify outside of range");
		dicAlarms.put(62, "Actuator goes outside the stroke limit specified by params");
		dicAlarms.put(96, "Reacting force is outside limit");
		dicAlarms.put(97, "Return to origin is not completed in time");
		dicAlarms.put(98, "While the servo is OFF operation is requested");
		dicAlarms.put(99, "Positioning or pushing operation is requested before the return origin is completed");
		dicAlarms.put(103, "Did not detect sensor when returning to ORIG");

		// those alarms are deactivated with RESET + SVON
		dicAlarms.put(144, "Speed exceeded set value");
		dicAlarms.put(145, "Motor power supply voltage is outside set range");
		dicAlarms.put(146, "Controller temp exceeded set range");
		dicAlarms.put(147, "Controller supply voltage is outside set range");
		dicAlarms.put(148, "Current limit is exceeded");
		dicAlarms.put(149, "The target osition was no reached in time");
		dicAlarms.put(150, "Communication error");

		// those alarms are deactivated reset power supply of the controller
		dicAlarms.put(192, "Encoder error");
		dicAlarms.put(193, "Unable to find motor phase in set time");
		dicAlarms.put(194, "Output current limit is exceeded set value");
		dicAlarms.put(196, "Error counter overflowed");
		dicAlarms.put(197, "Memory Adnormality has ocurred");
		dicAlarms.put(198, "CPU error");

	}

	public AlarmSMCServo() {

		super("no alarm SMC servo", 0);

		int n = (alarms.length > N_ALARMS ? N_ALARMS : alarms.length);

		for (int i = 0; i < n; i++) {

			this.alarms[i] = 0;

		}

	}

	public AlarmSMCServo(byte[] alarms) {

		super(getStringAlarm((int) alarms[0]), alarms[0]);

		int n = (alarms.length > N_ALARMS ? N_ALARMS : alarms.length);

		for (int i = 0; i < n; i++) {

			this.alarms[i] = (int) (0xFF & alarms[i]);

		}

	}

	public AlarmSMCServo(String description, byte[] alarms) {

		super(description, alarms[0]);

		int n = (alarms.length > N_ALARMS ? N_ALARMS : alarms.length);

		for (int i = 0; i < n; i++) {

			this.alarms[i] = (int) (0xFF & alarms[i]);

		}

	}

	public AlarmSMCServo(String description, int id) {
		
		super(description, id);
		
	}

	public static String getStringAlarm(int code) {

		if (dicAlarms.containsKey(code)) {

			return dicAlarms.get(code);

		}

		return "";

	}

	public int[] getAlarms() {

		return alarms;

	}

	@Override
	public String toString() {

		String strAlarms = "";
		String s;

		for (int i = 0; i < alarms.length; i++) {

			s = getStringAlarm((int) alarms[i]);

			if (!s.isEmpty())
				strAlarms += s;
			else
				break;

		}

		if (strAlarms.isEmpty())
			return "Unknown alarm, CODES: " + alarms[0] + ", " + alarms[1] + ", " + alarms[2] + ", " + alarms[3];
		else
			return strAlarms;

	}
}
