import matplotlib.pyplot as plt

x = [100, 1000, 10000, 20000, 30000]
seqLCS = [691, 116107, 1764093, 5665093, 12611471]
parLCS = [22466, 308249, 3420977, 8428458, 16464317]

plt.plot(x, list(zip(seqLCS, parLCS)), label=['sequential', 'parallel'])
plt.title('Sequential vs Parallel LCS comparison')
plt.xlabel('string length')
plt.ylabel('average execution time (10 measurements)')
plt.legend()
plt.show()
