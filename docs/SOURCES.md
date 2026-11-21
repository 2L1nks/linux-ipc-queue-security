# Первоисточники для курсовой работы

**Дата проверки доступности/актуальности ключевых материалов:** 21 ноября 2026 года. При подготовке финального отчёта проверить используемую версию каждой статьи и оформить ссылки согласно требованиям вуза. Здесь приведены устойчивые исходные адреса.

## Глава 1: теория IPC и реализация

1. [sysvipc(7), Linux manual pages](https://man7.org/linux/man-pages/man7/sysvipc.7.html) — обзор System V IPC.
2. [msgget(2)](https://man7.org/linux/man-pages/man2/msgget.2.html) — создание очереди и права.
3. [msgsnd(2)/msgrcv(2)](https://man7.org/linux/man-pages/man2/msgsnd.2.html) — передача/получение, `IPC_NOWAIT`, ошибки.
4. [msgctl(2)](https://man7.org/linux/man-pages/man2/msgctl.2.html) — `msqid_ds`, `IPC_STAT`, `IPC_SET`, `IPC_RMID`.
5. [mq_overview(7)](https://man7.org/linux/man-pages/man7/mq_overview.7.html) — POSIX очереди, приоритет, лимиты, персистентность.
6. [mq_open(3)](https://man7.org/linux/man-pages/man3/mq_open.3.html), [mq_send(3)](https://man7.org/linux/man-pages/man3/mq_send.3.html), [mq_receive(3)](https://man7.org/linux/man-pages/man3/mq_receive.3.html).
7. [Linux kernel: ipc/msg.c](https://github.com/torvalds/linux/blob/master/ipc/msg.c), [ipc/msgutil.c](https://github.com/torvalds/linux/blob/master/ipc/msgutil.c), [include/linux/msg.h](https://github.com/torvalds/linux/blob/master/include/linux/msg.h), [ipc/mqueue.c](https://github.com/torvalds/linux/blob/master/ipc/mqueue.c) — исходники; в отчёте фиксировать commit или версию релиза, поскольку ветка master меняется.
8. [The Linux Programming Interface](https://man7.org/tlpi/) — справочник M. Kerrisk (доступность текста — по лицензии/в библиотеке).

## Глава 2: разграничение доступа и ограничения

9. [ipc_namespaces(7)](https://man7.org/linux/man-pages/man7/ipc_namespaces.7.html) и [namespaces(7)](https://man7.org/linux/man-pages/man7/namespaces.7.html).
10. [capabilities(7)](https://man7.org/linux/man-pages/man7/capabilities.7.html).
11. [Kernel sysctl: kernel.msgmax/msgmnb/msgmni](https://docs.kernel.org/admin-guide/sysctl/kernel.html).
12. [Kernel sysctl: fs.mqueue](https://docs.kernel.org/admin-guide/sysctl/fs.html).
13. [getrlimit(2), RLIMIT_MSGQUEUE](https://man7.org/linux/man-pages/man2/getrlimit.2.html).
14. [CWE-362 race condition](https://cwe.mitre.org/data/definitions/362.html), [CWE-416 UAF](https://cwe.mitre.org/data/definitions/416.html), [CWE-672 expired resource](https://cwe.mitre.org/data/definitions/672.html).

## Глава 3: CVE и исправления

15. [NVD CVE-2015-7613](https://nvd.nist.gov/vuln/detail/CVE-2015-7613) и [исправление b9a532277938…](https://github.com/torvalds/linux/commit/b9a532277938798b53178d5a66af6e2915cb27cf) — race при инициализации System V объектов.
16. [NVD CVE-2017-11176](https://nvd.nist.gov/vuln/detail/CVE-2017-11176) и [исправление f991af3daaba…](https://github.com/torvalds/linux/commit/f991af3daabaecff34684fd51fac80319d1baad1) — UAF в `mq_notify`.
17. [NVD CVE-2021-47069](https://nvd.nist.gov/vuln/detail/CVE-2021-47069), [stable-патч 4528c0c32308…](https://git.kernel.org/stable/c/4528c0c323085e645b8765913b4a7fd42cf49b65) — гонка в IPC. Доступны разные backport-коммиты.
18. [Debian Security Tracker](https://security-tracker.debian.org/tracker/) — проверка статуса исправлений в дистрибутиве.
19. [CVE.org](https://www.cve.org/) — первичные CVE-записи.
20. [Linux kernel self-protection](https://docs.kernel.org/security/self-protection.html) — методы защиты ядра.
21. [CVE-2021-22555 write-up](https://google.github.io/security-research/pocs/linux/cve-2021-22555/writeup.html) — **только** как иллюстрация вспомогательной роли `msg_msg` в исследованиях других подсистем, не как CVE самого `ipc/msg.c`.

## Глава 4: воспроизводимость и диагностика

22. [ipcs(1)](https://man7.org/linux/man-pages/man1/ipcs.1.html), [ipcrm(1)](https://man7.org/linux/man-pages/man1/ipcrm.1.html).
23. [strace](https://strace.io/) — пользовательская трассировка системных вызовов.
24. [unshare(1)](https://man7.org/linux/man-pages/man1/unshare.1.html) — создание отдельного namespace.
25. [stress-ng](https://github.com/ColinIanKing/stress-ng) — дополнительный инструмент для нагрузочных исследований (основной проект обходится без него).
26. [VirtualBox manual](https://www.virtualbox.org/manual/) — изоляция и снимки ВМ.
27. [Linux kernel ftrace](https://docs.kernel.org/trace/ftrace.html) — факультативная трассировка.

## Глава 5: рекомендации

28. [Linux kernel admin guide](https://docs.kernel.org/admin-guide/) — эксплуатационные настройки.
29. [Kernel command-line parameters](https://docs.kernel.org/admin-guide/kernel-parameters.html) — средства защиты/загрузочные параметры; поддержка зависит от сборки.
30. [auditctl(8)](https://man7.org/linux/man-pages/man8/auditctl.8.html) — аудит событий.
31. [AppArmor documentation](https://apparmor.net/) — политики ограниченного доступа.
32. [SEI CERT C Coding Standard](https://wiki.sei.cmu.edu/confluence/display/c/SEI+CERT+C+Coding+Standard) — практики безопасного кода.

## Как оформлять ссылки в отчёте

Ссылаться на документ/коммит и дату обращения, а не только на поисковый результат. Для исходников ядра указать конкретный тег или commit hash; для CVE — NVD URL, дату обращения, отдельную ссылку на патч; для программ C — путь к файлу и commit GitHub. Не копировать длинные фрагменты источников без цитирования.
