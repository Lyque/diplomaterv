/*
 * measure_config.h
 *
 *  Created on: 2017. m�rc. 6.
 *      Author: Lyque
 */

#ifndef MEASURE_CONFIG_H_
#define MEASURE_CONFIG_H_

/**
 * M�r�si folyamat:
 * 	MEAS_LATENCY: 							k�sleltet�s m�r�s
 * 	MEAS_TASK_SWITCHING_TIME: 				taszkv�lt�si id� m�r�s
 * 	MEAS_PREEMPTION_TIME: 					preemt�l�si id� m�r�s
 * 	MEAS_INTERRUPT_LATENCY_TIME:			megszak�t�s-k�sleltet�si id� m�r�s
 * 	MEAS_SEMAPHORE_SHUFFLING_TIME:			szemafor-v�lt�si id� m�r�s
 * 	MEAS_DEADLOCK_BREAKING_TIME:			deadlock-felold�si id� m�r�s
 * 	MEAS_DATAGRAM_THROUGHPUT_TIME:			datagram-�tviteli id� m�r�s
 */
/**
 * Terhel�s enged�lyez�se:
 *	MEAS_W_LOAD
 */
#define MEAS_DATAGRAM_THROUGHPUT_TIME
#define BLINKING_LED
#define MEAS_W_LOAD

/**
 * A task ID-k A-t�l O-ig mehetnek.
 * A/a: 0x1	I/i: 0x9
 * B/b: 0x2	J/j: 0xA
 * C/c: 0x3	K/k: 0xB
 * D/d: 0x4	L/l: 0xC
 * E/e: 0x5	M/m: 0xD
 * F/f: 0x6	N/n: 0xE
 * G/g: 0x7	O/o: 0xF
 * H/h: 0x8
 *
 * �temez�: 0x0
 */
#define IDLETASKID						"A"
#define STARTMEASURETASKID				B

#if defined(MEAS_TASK_SWITCHING_TIME)
#define SWITCHINGTIMETASKAID 			C
#define SWITCHINGTIMETASKBID 			D
#define SWITCHINGTIMETASKCID 			E
#endif

#if defined(MEAS_PREEMPTION_TIME)
#define PREEMPTIONTIMETASKAID 			F
#define PREEMPTIONTIMETASKBID 			G
#define PREEMPTIONTIMETASKCID 			H
#endif

#if defined(MEAS_INTERRUPT_LATENCY_TIME)
#define INTERRUPTLATENCYTIMETASKID 		I
#endif

#if defined(MEAS_SEMAPHORE_SHUFFLING_TIME)
#define SEMAPHORESHUFFLINGTIMETASKAID 	J
#define SEMAPHORESHUFFLINGTIMETASKBID 	K
#endif

#if defined(MEAS_DEADLOCK_BREAKING_TIME)
#define DEADLOCKBREAKINGTIMETASKAID		L
#define DEADLOCKBREAKINGTIMETASKBID		M
#define DEADLOCKBREAKINGTIMETASKCID		N
#endif

#if defined(MEAS_DATAGRAM_THROUGHPUT_TIME)
#define DATAGRAMTHROUGHPUTTIMETASKAID	O
#define DATAGRAMTHROUGHPUTTIMETASKBID	n
#endif

#if defined(BLINKING_LED)
#define FORWARDLEDTASKID				a
#define REVERSELEDTASKID				b
#endif

#if defined(MEAS_W_LOAD)
#define SWITCHCHANGEDTASKID				c
#define TEMPMEASTASKID					d
#define POTMETERMEASTASKID				e
#define BLETASKID						f
#define UART6TASKID						g
#define SDCARDTASKID					h
#endif

#define LOW_PRIORITY_TASK_LOOP_DELAY		500000
#define MEDIUM_PRIORITY_TASK_LOOP_DELAY		250000
#define HIGH_PRIORITY_TASK_LOOP_DELAY		50000

#define EXPAND(...) __VA_ARGS__
#define ThreadDef(name, thread, priority, instances, stacksz) osThreadDef(name, thread, priority, instances, stacksz)
#define Thread(name) osThread(name)

#endif /* MEASURE_CONFIG_H_ */
