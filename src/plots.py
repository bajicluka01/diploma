import matplotlib.pyplot as plt

x = [10000, 20000, 30000, 40000, 50000, 60000]
seqLevenshtein = [0.549787, 1.901921, 3.791124, 5.911732, 9.231984, 14.285577]
parLevenshtein = [0.255227, 1.174503, 2.675816, 4.022211, 6.941683, 9.575530]

plt.plot(x, list(zip(seqLevenshtein, parLevenshtein)), label=['forward', 'forward-backward'])
plt.title('Forward vs Forward-backward Levenshtein comparison')
plt.xlabel('string length')
plt.ylabel('average execution time (5 measurements)')
plt.legend()
plt.show()
