import matplotlib.pyplot as plt
import numpy as np 

x = [10000, 20000, 30000, 40000, 50000, 60000]
x2 = [100000, 150000, 200000, 250000, 300000, 350000, 400000]
x3 = [10000, 20000, 30000, 40000, 50000]
x4 = [100000, 200000, 300000, 400000, 500000, 600000, 700000]

x5 = np.arange(7) 

forwardLevenshtein =                [0.345842, 1.519301, 3.626702, 6.293141, 9.199267, 13.429229]
backwardLevenshtein =               [0.338444, 1.529102, 3.404311, 6.001849, 9.053014, 13.321899]
diagonalLevenshteinMemory =         [0.239417, 0.943647, 2.102897, 3.743161, 5.900386, 9.689873]
fbLevenshtein =                     [0.258670, 1.187424, 2.500014, 4.317889, 6.505424, 9.554413] #2 threads
diagonalLevenshteinMemoryParallel = [0.831118, 1.856795, 2.899954, 3.889169, 5.475029, 7.340391] #8 threads

forwardLevenshteinSpace =          [12.817226, 29.442941, 51.170906, 79.912627, 118.889369, 159.856438, 210.639703]
backwardLevenshteinSpace =         [12.366502, 27.420466, 50.098321, 77.801877, 112.954502, 153.924200, 201.562494]
diagonalLevenshteinSpace =         [12.954458, 26.667030, 48.488514, 75.765589, 113.369859, 150.815187, 197.959408]
fbLevenshteinSpace =               [8.291492,  18.413865, 33.197180, 50.139865, 81.362590,  109.328794, 144.999530] #2 threads
diagonalLevenshteinSpaceParallel = [12.361829, 22.076464, 38.742937, 50.713466, 69.038534,  94.300314,  117.904765] #8 threads

forwardLCS =                [0.319667, 1.429334, 3.211196, 6.018281, 10.109169, 12.394485]
backwardLCS =               [0.318628, 1.438086, 3.223706, 6.271317, 9.850569,  12.581315]
diagonalLCSmemory =         [0.221382, 0.924055, 1.938263, 3.413420, 5.299615,  7.664671]
fbLCS =                     [0.247322, 1.136386, 2.406986, 4.189387, 7.354680,  9.246377] #2 threads
diagonalLCSmemoryParallel = [0.841188, 1.865099, 3.037416, 4.297242, 5.603433,  7.296277] #6 threads

forwardLCSspace =          [11.047188, 23.489699, 41.342735, 65.269299, 104.050567, 141.976719, 188.242982]
backwardLCSspace =         [10.564286, 23.267623, 42.785099, 68.833067, 98.918783,  132.945592, 176.712915]
diagonalLCSspace =         [11.285618, 24.405456, 44.325217, 69.097606, 101.582317, 138.790737, 177.593065]
fbLCSspace =               [6.365848,  14.281909, 26.146862, 40.857753, 64.107716,  86.491611,  114.585233] #2 threads
diagonalLCSspaceParallel = [11.613160, 21.806565, 34.844932, 50.740788, 70.534725,  93.739369,  119.876094] #6 threads

fbLCSspaceExtended =               [6.365848,  26.146862, 64.107716, 114.585233, 178.351895, 260.908079, 357.052496]
diagonalLCSspaceParallelExtended = [11.613160, 34.844932, 70.534725, 119.876094, 190.159906, 254.635486, 329.583547]


#the old function, not relevant anymore
diagonalLevenshtein =         [0.766702, 3.249734, 7.991920, 16.164053, 35.703142]
diagonalLevenshteinParallel = [1.018414, 2.634201, 5.033297, 8.543985,  17.881329]

nThreads2 =  [1.307152, 4.930329, 10.830216, 36.209834, 55.973594]
nThreads3 =  [1.049671, 3.664200, 8.004958,  14.237673, 34.572387]
nThreads4 =  [0.872162, 2.981476, 6.441798,  11.365335, 26.947444]
nThreads5 =  [0.846055, 2.625845, 5.494065,  9.388782,  19.622980]
nThreads6 =  [0.846184, 2.393235, 4.984522,  8.507192,  17.990511]
nThreads7 =  [0.928700, 2.387165, 4.719933,  8.053903,  15.864173]
nThreads8 =  [1.031289, 2.458716, 4.608730,  7.937492,  16.417606]
nThreads9 =  [1.253855, 3.000064, 5.682924,  9.514016,  17.121239]
nThreads10 = [1.451054, 3.191380, 5.892956,  9.580704,  18.361809]
nThreads11 = [1.669980, 3.346404, 5.933924,  9.661376,  17.336488]
nThreads12 = [1.747410, 3.475634, 6.001397,  9.517540,  17.887280]

#Levenshtein

f = plt.figure(7)
ax = f.add_subplot(111)
ax.yaxis.tick_right()
plt.plot(x, list(zip(forwardLevenshtein, backwardLevenshtein, fbLevenshtein, diagonalLevenshteinMemory, diagonalLevenshteinMemoryParallel)), label=['naprej', 'nazaj', 'naprej-nazaj', 'diagonalni', 'diagonalni vzporedni (8 niti)'])
plt.title('Primerjava algoritmov za računanje Levenshteinove razdalje')
plt.xlabel('dolžina niza')
plt.ylabel('čas izvajanja (sekunde)')
plt.legend()

plt.figure(1)
plt.plot(x, list(zip(forwardLevenshtein, backwardLevenshtein, fbLevenshtein, diagonalLevenshteinMemory, diagonalLevenshteinMemoryParallel)), label=['naprej', 'nazaj', 'naprej-nazaj', 'diagonalni', 'diagonalni vzporedni (8 niti)'])
plt.title('Primerjava algoritmov za računanje Levenshteinove razdalje')
plt.xlabel('dolžina niza')
plt.ylabel('čas izvajanja (sekunde)')
plt.legend()

plt.figure(2)
plt.plot(x2, list(zip(forwardLevenshteinSpace, backwardLevenshteinSpace, fbLevenshteinSpace, diagonalLevenshteinSpace, diagonalLevenshteinSpaceParallel)), label=['naprej', 'nazaj', 'naprej nazaj', 'diagonalni', 'diagonalni vzporedni (8 niti)'])
plt.title('Primerjava algoritmov za računanje Levenshteinove razdalje s prostorsko optimizacijo')
plt.xlabel('dolžina niza')
plt.ylabel('čas izvajanja (sekunde)')
plt.legend()

plt.figure(3)
plt.plot(x3, list(zip(diagonalLevenshtein, nThreads3, nThreads4, nThreads5, nThreads6, nThreads7, nThreads8, nThreads9, nThreads10, nThreads11, nThreads12)), label=['osnovni algoritem (1 nit)', '3 niti', '4 niti', '5 niti', '6 niti', '7 niti', '8 niti', '9 niti', '10 niti', '11 niti', '12 niti'])
plt.title('Levenshteinova razdalja, diagonalni pristop: primerjava glede na število niti')
plt.xlabel('dolžina niza')
plt.ylabel('čas izvajanja (sekunde)')
plt.legend()


# LCS

plt.figure(4)
plt.plot(x, list(zip(forwardLCS, backwardLCS, fbLCS, diagonalLCSmemory, diagonalLCSmemoryParallel)), label=['naprej', 'nazaj', 'naprej-nazaj', 'diagonalni', 'diagonalni vzporedni (6 niti)'])
plt.title('Primerjava algoritmov za računanje LCS')
plt.xlabel('dolžina niza')
plt.ylabel('čas izvajanja (sekunde)')
plt.legend()

plt.figure(5)
plt.plot(x2, list(zip(forwardLCSspace, backwardLCSspace, fbLCSspace, diagonalLCSspace, diagonalLCSspaceParallel)), label=['naprej', 'nazaj', 'naprej nazaj', 'diagonalni', 'diagonalni vzporedni (6 niti)'])
plt.title('Primerjava algoritmov za računanje LCS s prostorsko optimizacijo')
plt.xlabel('dolžina niza')
plt.ylabel('čas izvajanja (sekunde)')
plt.legend()

plt.figure(6)
plt.plot(x4, list(zip(fbLCSspaceExtended, diagonalLCSspaceParallelExtended)), label=['naprej nazaj', 'diagonalni vzporedni (6 niti)'])
plt.title('Primerjava algoritmov za računanje LCS s prostorsko optimizacijo')
plt.xlabel('dolžina niza')
plt.ylabel('čas izvajanja (sekunde)')
plt.legend()


width = 0.1
plt.figure(8)
plt.bar(x5-0.2, forwardLevenshteinSpace, width, color='brown') 
plt.bar(x5-0.1, backwardLevenshteinSpace, width, color='gray') 
plt.bar(x5, fbLevenshteinSpace, width, color='pink') 
plt.bar(x5+0.1, diagonalLevenshteinSpace, width, color='olive') 
plt.bar(x5+0.2, diagonalLevenshteinSpaceParallel, width, color='cyan') 
plt.xticks(x5, x2) 
plt.title('Primerjava algoritmov za računanje Levenshteinove razdalje s prostorsko optimizacijo')
plt.xlabel("dolžina niza") 
plt.ylabel("čas izvajanja (sekunde)") 
plt.legend(['naprej', 'nazaj', 'naprej-nazaj', 'diagonalni', 'diagonalni vzporedni (8 niti)'])

plt.show()
