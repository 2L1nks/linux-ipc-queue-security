# Чек-лист скриншотов для отчёта

Скриншоты делают **на Kali Linux 2026.2**, а не из заранее сгенерированного примера. Все файлы сохранять с номерами и понятными названиями в `screenshots/`. Подойдут PNG; перед публикацией проверить приватную информацию.

| № | Предлагаемое имя | Где / что показать |
| --- | --- | --- |
| 01 | `01_project_structure.png` | `pwd`, `ls -la`, имя папки репозитория |
| 02 | `02_vm_snapshot.png` | гипервизор: ВМ и снимок `ipc-before-tests` |
| 03 | `03_environment_limits.png` | `uname -r`, `/etc/os-release`, `sysctl kernel.msg*` |
| 04 | `04_compilation.png` | `make`, компиляция без ошибок |
| 05 | `05_basic_queues.png` | `./bin/sysv_demo`, `./bin/posix_demo`, обе строки `RECEIVED` |
| 06 | `06_sysv_strace.png` | System V `strace` с `msgsnd`/`msgrcv` |
| 07 | `07_posix_strace.png` | POSIX `strace` с `mq_timedsend`/`mq_timedreceive` |
| 08 | `08_sysv_limits.png` | System V режимы 4096 и 2048, `errno_full=11` |
| 09 | `09_posix_limits.png` | POSIX 4 и 2 сообщения, `errno_full=11` и `EMSGSIZE` |
| 10 | `10_run_experiments.png` | вывод `run_experiments.sh` с путём каталога |
| 11 | `11_resource_snapshots.png` | BEFORE/DURING/AFTER из `resource_state.log`, `ipcs -u` |
| 12 | `12_limit_graph.png` | открытый `limit_comparison.svg` с подписями |
| 13 | `13_dac_denied.png` | `./scripts/permissions_test.sh`, `DENIED EXPECTED` |
| 14 | `14_ipc_namespace.png` | `namespace_test.sh`, различие IPC namespaces и `HIDDEN`, либо отказ `unshare` |
| 15 | `15_namespace_sysctl.png` | `sysctl_namespace_test.sh`, 2048/4, `ENOSPC` и восстановленные значения, либо отказ |
| 16 | `16_cleanup.png` | `ipcs -q` и `/dev/mqueue`, отсутствие ваших тестовых объектов |

**Подсказка:** в XFCE Kali можно использовать `Print Screen` или `xfce4-screenshooter -f` и сохранить изображение в `screenshots/`. Консольный вывод не редактировать так, чтобы менялись числа/ошибки; допустимо скрыть персональные данные вне области опыта.

## Как сохранять доказательства

1. После каждого этапа сделать скриншот, проверить читаемость шрифта и название команды.
2. Для опытов с отказом `unshare` скриншот содержит именно отказ. В тексте отчёта указать **«не выполнено»** и объяснить причину.
3. Не выкладывать в публичный GitHub изображения с токенами GitHub, электронными адресами, внешними IP и другими закрытыми данными.
4. Для отчёта подписывать как `Рисунок N — ...`, указывать источник «составлено автором по результатам эксперимента» и дату.
