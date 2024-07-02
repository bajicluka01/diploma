import matplotlib.pyplot as plt

x = [10000, 20000, 30000, 40000, 50000]
seqLevenshtein = [1.510601, 5.958291, 14.738912, 24.580494, 39.005777]
parLCS = [0.944706, 3.887717, 10.566906, 16.454426, 28.876415]

plt.plot(x, list(zip(seqLevenshtein, parLCS)), label=['forward', 'forward-backward'])
plt.title('Forward vs Forward-backward Levenshtein comparison')
plt.xlabel('string length')
plt.ylabel('average execution time (3 measurements)')
plt.legend()
plt.show()
