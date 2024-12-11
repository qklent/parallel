Коллективная передача сообщений в MPI - это механизм, который позволяет одновременно передавать данные между многими процессами в рамках одной операции. В отличие от точка-точка коммуникации (p2p), где сообщения передаются между двумя конкретными процессами, коллективные операции вовлекают все процессы в коммуникаторе.

Основные типы коллективных операций:

1. **Широковещательная рассылка (Broadcast) - MPI_Bcast**
```c
MPI_Bcast(void* data, int count, MPI_Datatype datatype, int root, MPI_Comm comm)
```
- Один процесс (root) рассылает одни и те же данные всем остальным процессам
```
Root процесс: [A]
              ↓
Все процессы: [A][A][A][A]
```

2. **Сбор данных (Gather) - MPI_Gather**
```c
MPI_Gather(void* send_data, int send_count, MPI_Datatype send_datatype,
          void* recv_data, int recv_count, MPI_Datatype recv_datatype,
          int root, MPI_Comm comm)
```
- Собирает данные со всех процессов в один массив на процессе-получателе (root)
```
Все процессы: [A][B][C][D]
                   ↓
Root процесс:  [A,B,C,D]
```

3. **Распределение данных (Scatter) - MPI_Scatter**
```c
MPI_Scatter(void* send_data, int send_count, MPI_Datatype send_datatype,
           void* recv_data, int recv_count, MPI_Datatype recv_datatype,
           int root, MPI_Comm comm)
```
- Разделяет массив данных с root-процесса между всеми процессами
```
Root процесс:  [A,B,C,D]
                   ↓
Все процессы: [A][B][C][D]
```

4. **Редукция (Reduce) - MPI_Reduce**
```c
MPI_Reduce(void* send_data, void* recv_data, int count, MPI_Datatype datatype,
          MPI_Op op, int root, MPI_Comm comm)
```
- Выполняет операцию (например, сумму) над данными всех процессов и сохраняет результат на root-процессе
```
Все процессы: [1][2][3][4]
                   ↓
Root процесс:    [10]  // сумма всех значений
```

Преимущества коллективных операций:

1. **Эффективность**: 
   - Оптимизированы для работы с множеством процессов