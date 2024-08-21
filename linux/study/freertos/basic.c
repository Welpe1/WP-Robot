/**
 * FreeRTOS的两个重要的数据类型
 * TickType_t
 * BaseType_t 通常用作简单的返回值的类型，还有逻辑值
 * 
 */

/**
 * FreeRTOS的任务（必须是死循环且无返回值的）有四种状态
 * Running 
 * Ready 
 * Blocked（阻塞） 任务要等待某个事件发生后才能运行。在等待事件过程中不消耗CPU资源
 * Suspended（挂起）任务暂停
 * 
 * 
 * IDLE任务（空闲任务）
 * @brief 除了用户定义的任务外，还有IDLE任务（空闲任务），其优先级最低，由内核自动创建和管理
 *        当使用vTaskStartScheduler()函数启动调度器时，该函数内部会负责创建IDLE任务
 *        主要作用：防止处理器空转、资源回收、执行轻量级操作等
 *        注意：使用vTaskDelete()来删除任务要确保空闲任务有机会执行，否则无法释放被删除任务的内存
 * 
 * Idle Task Hook Functions（钩子函数）
 * @brief 空闲任务的循环每执行一次，就会调用一次钩子函数
 *        主要作用：执行一些低优先级的、后台的、需要连续执行的函数等
 * 
 *                 
 */



/**
 * FreeRTOS创建任务时需要两个内核对象 stack和TCB
 * 
 * stack（栈）
 * @brief 任务执行时存储临时数据和函数调用信息的关键区域
 *        主要作用包括存储局部变量、保存寄存器状态、支持函数调用、临时数据存储等
 * 
 * 
 * TCB（任务控制块）
 * @brief 一个集中存储了与任务相关的各种信息的数据结构，使内核能有效管理和调度这些任务，实现任务调度
 *        主要作用包括存储任务状态、保存任务栈指针、存储任务优先级、提供任务名称、支持任务执行时间统计等
 * @param pxTopOfStack（栈顶指针）
 * @param xStateListItem（状态列表项） 记录TCB所在的状态列表，如就绪列表、当前运行列表或等待列表
 * @param uxPriority（优先级）
 * @param pxStack（任务栈） 指向任务栈起始地址的指针
 * @param pcTaskName（任务名称）
 * 
 */



/**
 * 常用函数
 * 
 * xTaskCreate 
 * @brief 创建任务（动态分配内存）
 * @param pxTaskCode 指向任务函数的指针 
 * @param pcName 任务名
 * @param pvParameters 调用任务函数时传入的参数
 * @param usStackDepth 栈大小 
 * @param pvParameters 优先级
 * @param pxCreatedTask 任务句柄
 * 
 * vTaskDelete
 * @brief 删除任务
 * @param xTaskToDelete
 * 
 * vTaskSuspend
 * @brief 使任务进入挂起状态
 * 
 * pvPortMalloc
 * @brief 分配内存
 * @param xWantedSize 
 * 
 * vPortFree
 * @brief 释放内存
 * @param pv
 * 
 * 
 */


/**
 * 信号或数据传递的方法
 * queue（队列） 在任务之间、任务和中断之间均可传输大小数据
 * 
 * xQueueCreate
 * @brief 动态分配内存创建队列
 * @param uxQueueLength 队列长度
 * @param uxItemSize 每个数据(item)的大小，单位为字节
 * 
 * vQueueDelete( QueueHandle_t  );
 * @brief 删除使用动态方法创建的队列
 * @param xQueue 队列句柄
 * 
 * xQueueSend xQueueSendToBack
 * @brief 往队列尾部写入数据
 * @param xQueue 队列句柄
 * @param pvItemToQueue 数据指针
 * @param xTicksToWait 如果队列满则阻塞时间为xTicksToWait
 * 
 * xQueueOverwrite
 * @brief 向队列写入数据（当队列满了以后自动覆盖旧信息）
 * 
 * xQueueReset
 * @brief 将队列重新设置为初始状态，即清空队列中的所以数据项，使其变成空队列
 * 
 * xQueueReceive
 * @brief 读队列
 * @param xQueue 队列句柄
 * @param pvBuffer bufer指针，队列的数据会被复制到这个buffer
 * @param xTicksToWait 如果队列空则阻塞时间为xTicksToWait
 * 
 * 
 * xQueuePeek
 * @brief 偷看队列（可以理解为读取时不移除数据）
 * @param xQueue 队列句柄
 * @param pvBuffer bufer指针，队列的数据会被复制到这个buffer
 * @param xTicksToWait 如果队列空则阻塞时间为xTicksToWait
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * semaphore（信号量） 包括二进制信号量和计数信号量
 * mutex（互斥锁）
 * event group（事件组） 具有广播作用
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 */







