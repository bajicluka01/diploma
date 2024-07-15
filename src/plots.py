import matplotlib.pyplot as plt

x = [10000, 20000, 30000, 40000, 50000, 60000]
x2 = [100000, 150000, 200000, 250000]

forwardLevenshtein =  [0.345842, 1.519301, 3.626702, 6.293141, 9.199267, 13.429229]
backwardLevenshtein = [0.338444, 1.529102, 3.404311, 6.001849, 9.053014, 13.321899]
fbLevenshtein =       [0.258670, 1.187424, 2.500014, 4.317889, 6.505424, 9.554413]

forwardLevenshteinSpace =  [26.395551, 58.962033, 105.193894, 167.749272]
backwardLevenshteinSpace = [24.271178, 54.160883, 95.668674, 149.694305]
fbLevenshteinSpace =       [13.435573, 30.358760, 53.150599, 83.564126]

forwardLCS =  [0.319667, 1.429334, 3.211196, 6.018281, 10.109169, 12.394485]
backwardLCS = [0.318628, 1.438086, 3.223706, 6.271317, 9.850569, 12.581315]
fbLCS =       [0.247322, 1.136386, 2.406986, 4.189387, 7.354680, 9.246377]

forwardLCSspace =  [11.047188, 23.489699, 41.342735, 65.269299]
backwardLCSspace = [10.564286, 23.267623, 42.785099, 68.833067]
fbLCSspace =       [6.365848, 14.281909, 26.146862, 40.857753]

#TODO
diagonalLevenshtein = []
diagonalLCS =         []

plt.figure(1)
plt.plot(x, list(zip(forwardLevenshtein, backwardLevenshtein, fbLevenshtein)), label=['forward', 'backward', 'forward-backward'])
plt.title('Levenshtein comparison')
plt.xlabel('string length')
plt.ylabel('average execution time (seconds)')
plt.legend()


plt.figure(2)
plt.plot(x, list(zip(forwardLCS, backwardLCS, fbLCS)), label=['forward', 'backward', 'forward-backward'])
plt.title('LCS comparison')
plt.xlabel('string length')
plt.ylabel('average execution time (seconds)')
plt.legend()


plt.figure(3)
plt.plot(x2, list(zip(forwardLevenshteinSpace, backwardLevenshteinSpace, fbLevenshteinSpace)), label=['forward', 'backward', 'forward-backward'])
plt.title('Levenshtein comparison with space optimization')
plt.xlabel('string length')
plt.ylabel('average execution time (seconds)')
plt.legend()


plt.figure(4)
plt.plot(x2, list(zip(forwardLCSspace, backwardLCSspace, fbLCSspace)), label=['forward', 'backward', 'forward-backward'])
plt.title('LCS comparison with space optimization')
plt.xlabel('string length')
plt.ylabel('average execution time (seconds)')
plt.legend()


plt.show()
