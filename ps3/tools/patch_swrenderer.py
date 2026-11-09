import re, sys
SRC = '../juego-original/OpenTESArena/src/Rendering/SoftwareRenderer.cpp'
DST = 'compat/SoftwareRenderer.cpp'
s = open(SRC, encoding='utf-8', newline='').read().replace('\r\n', '\n')

def sub(pattern, repl, label):
    global s
    s, n = re.subn(pattern, lambda m: repl, s, count=1)
    print(('OK    ' if n == 1 else 'FALLO '), label)
    if n != 1:
        sys.exit(1)

for h in ('condition_variable', 'mutex', 'thread'):
    sub(r'#include <%s>\n' % h, '', 'include ' + h)
sub(r'[ \t]*std::thread thread;\n', '', 'Worker.thread')
sub(r'[ \t]*std::mutex g_mutex;\n[ \t]*std::condition_variable g_workerCondVar, g_directorCondVar;\n', '', 'globales mutex/condvar')

a = s.index('void WorkerFunc(int workerIndex)')
b = s.index('void SignalWorkersToExitAndJoin()')
t = s[a:b]
d0 = t.index('for (int drawCallIndex = 0; drawCallIndex < worker.drawCallCount; drawCallIndex++)')
d1 = t.index('workerLock.lock();', d0)
r0 = t.index('// Use the geometry processing results')
r1 = t.index('workerLock.lock();', r0)
head = '\n\t{\n\t\tWorker &worker = g_workers.get(workerIndex);\n\t\t'
new = ('void WorkerDrawPhase(int workerIndex)' + head + t[d0:d1] + '\n\t}\n\n\t'
       'void WorkerRasterPhase(int workerIndex)' + head + t[r0:r1] + '\n\t}\n\n\t')
s = s[:a] + new + s[b:]
print('OK     WorkerFunc dividida en dos fases')

a = s.index('void SignalWorkersToExitAndJoin()')
b = s.index('void InitializeWorkers(')
s = s[:a] + 'void SignalWorkersToExitAndJoin()\n\t{\n\t}\n\n\t' + s[b:]
print('OK     SignalWorkersToExitAndJoin vacia')

sub(r'[ \t]*worker\.thread = std::thread\(WorkerFunc, workerIndex\);\n', '', 'creacion de hilos')
sub(r'[ \t]*std::unique_lock<std::mutex> lock\(g_mutex\);\n', '', 'lock del director')

def waitpat(member):
    return (r'g_directorCondVar\.wait\(lock, \[\]\(\)\s*\{\s*return std::all_of\(g_workers\.begin\(\), '
            r'g_workers\.end\(\), \[\]\(const Worker &worker\) \{ return worker\.%s; \}\);\s*\}\);' % member)

sub(waitpat('isReadyToStartWork'), '', 'espera ready')
sub(r'g_workerCondVar\.notify_all\(\);\s*' + waitpat('isFinishedWithDrawCalls'),
    'for (int wi = 0; wi < totalWorkerCount; wi++) { WorkerDrawPhase(wi); }', 'fase geometria')
sub(r'g_workerCondVar\.notify_all\(\);\s*' + waitpat('isFinishedRasterizing'),
    'for (int wi = 0; wi < totalWorkerCount; wi++) { WorkerRasterPhase(wi); }', 'fase raster')
sub(r'const int totalWorkerCount = RendererUtils::getRenderThreadsFromMode\(settings\.renderThreadsMode\);',
    'const int totalWorkerCount = 1;', 'un solo trabajador')

for w in ('std::thread', 'std::mutex', 'CondVar', 'unique_lock'):
    print('restos de', w, ':', s.count(w))
open(DST, 'w', encoding='utf-8').write(s)
print('escrito', DST)
