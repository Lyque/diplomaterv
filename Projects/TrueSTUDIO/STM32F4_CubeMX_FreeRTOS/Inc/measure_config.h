/*
 * measure_config.h
 *
 *  Created on: 2017. márc. 6.
 *      Author: Lyque
 */

#ifndef MEASURE_CONFIG_H_
#define MEASURE_CONFIG_H_

// A task ID-k A-tól O-ig mehetnek.
#define IDLETASKID			"A"
#define DEFAULTTASKID		B
#define REVERSETASKID		D
#define UART6TASKID			H
#define EXPAND(...) __VA_ARGS__
#define ThreadDef(name, thread, priority, instances, stacksz) osThreadDef(name, thread, priority, instances, stacksz)
#define Thread(name) osThread(name)

#endif /* MEASURE_CONFIG_H_ */
