/*
 * measure_config.h
 *
 *  Created on: 2017. márc. 6.
 *      Author: Lyque
 */

#ifndef MEASURE_CONFIG_H_
#define MEASURE_CONFIG_H_

/**
 * Mérési folyamat:
 * 	MEAS_LATENCY: 							késleltetés mérés
 * 	MEAS_TASK_SWITCHING_TIME: 				taszkváltási idõ mérés
 * 	MEAS_PREEMPTION_TIME: 					preemtálási idõ mérés
 * 	MEAS_INTERRUPT_LATENCY_TIME:			megszakítás-késleltetési idõ mérés
 * 	MEAS_SEMAPHORE_SHUFFLING_TIME:			szemafor-váltási idõ mérés
 * 	MEAS_DEADLOCK_BREAKING_TIME:			deadlock-feloldási idõ mérés
 * 	MEAS_DATAGRAM_THROUGHPUT_TIME:			datagram-átviteli idõ mérés
 */
/**
 * Terhelés engedélyezése:
 *	MEAS_W_LOAD
 */
#define MEAS_DATAGRAM_THROUGHPUT_TIME
//#define BLINKING_LED
#define MEAS_W_LOAD

/**
 * A task ID-k A-tól O-ig mehetnek.
 * A/a: 0x1	I/i: 0x9
 * B/b: 0x2	J/j: 0xA
 * C/c: 0x3	K/k: 0xB
 * D/d: 0x4	L/l: 0xC
 * E/e: 0x5	M/m: 0xD
 * F/f: 0x6	N/n: 0xE
 * G/g: 0x7	O/o: 0xF
 * H/h: 0x8
 *
 * Ütemezõ: 0x0
 */

// ToDo: A taszkok ID-jait jól megválasztani!
#define IDLETASKID						"A"
#define STARTMEASURETASKID				B

#if defined(MEAS_TASK_SWITCHING_TIME)
#define SWITCHINGTIMETASKAID 			C
#define SWITCHINGTIMETASKBID 			D
#define SWITCHINGTIMETASKCID 			E
#endif

#if defined(MEAS_PREEMPTION_TIME)
#define PREEMPTIONTIMETASKAID 			C
#define PREEMPTIONTIMETASKBID 			D
#define PREEMPTIONTIMETASKCID 			E
#endif

#if defined(MEAS_INTERRUPT_LATENCY_TIME)
#define INTERRUPTLATENCYTIMETASKID 		C
#endif

#if defined(MEAS_SEMAPHORE_SHUFFLING_TIME)
#define SEMAPHORESHUFFLINGTIMETASKAID 	C
#define SEMAPHORESHUFFLINGTIMETASKBID 	D
#endif

#if defined(MEAS_DEADLOCK_BREAKING_TIME)
#define DEADLOCKBREAKINGTIMETASKAID		C
#define DEADLOCKBREAKINGTIMETASKBID		D
#define DEADLOCKBREAKINGTIMETASKCID		E
#endif

#if defined(MEAS_DATAGRAM_THROUGHPUT_TIME)
#define DATAGRAMTHROUGHPUTTIMETASKAID	C
#define DATAGRAMTHROUGHPUTTIMETASKBID	D
#endif

#if defined(BLINKING_LED)
#define FORWARDLEDTASKID				F
#define REVERSELEDTASKID				f
#endif

#if defined(MEAS_W_LOAD)
#define SWITCHCHANGEDTASKID				G
#define TEMPMEASTASKID					H
#define POTMETERMEASTASKID				I
#define BLETASKID						J
#define BLESENDTASKID					K
#define BLERECEIVETASKID				L
#define UART6TASKID						M
#define UART6SENDTASKID					N
#define SDCARDTASKID					O
#endif

#define LOW_PRIORITY_TASK_LOOP_DELAY		300000
#define MEDIUM_PRIORITY_TASK_LOOP_DELAY		100000
#define HIGH_PRIORITY_TASK_LOOP_DELAY		50000

#define BLEBAUDRATE							115200
#define UART6BAUDRATE						115200

#define EXPAND(...) __VA_ARGS__
#define ThreadDef(name, thread, priority, instances, stacksz) osThreadDef(name, thread, priority, instances, stacksz)
#define Thread(name) osThread(name)

// Alkalmazásspecifikus makrók

#define MESSAGE_LENGTH					ENTITY_LENGTH+VALUE_LENGTH+2
#define ENTITY_LENGTH					8
#define VALUE_LENGTH					4

// BLE-specifikus makrók
#define SENSORTAG_ADDRESS_0				0x01
#define SENSORTAG_ADDRESS_1				0x47
#define SENSORTAG_ADDRESS_2				0xC1
#define SENSORTAG_ADDRESS_3				0x89
#define SENSORTAG_ADDRESS_4				0x71
#define SENSORTAG_ADDRESS_5				0x24

// Reset
#define RESETTOMAINPROGRAM				0
#define RESETTODFU						1

// Connect direct
#define BLECONNINTERVALMIN				6
#define BLECONNINTERVALMAX				3200
#define BLECONNTIMEOUT					1000
#define BLECONNSLAVELATENCY				0

// Temperature
#define BLETEMPDATAHNDL					0x0024
#define BLETEMPCONFIGHNDL				0x0027
#define BLETEMPPERIODHNDL				0x0029

// Light
#define BLELIGHTDATAHNDL				0x0044
#define BLELIGHTCONFIGHNDL				0x0047
#define BLELIGHTPERIODHNDL				0x0049

#endif /* MEASURE_CONFIG_H_ */
