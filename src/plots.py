import matplotlib.pyplot as plt

x = [10000, 20000, 30000, 40000, 50000, 60000]

forwardLevenshtein = [0.549787, 1.901921, 3.791124, 5.911732, 9.231984, 14.285577]
backwardLevenshtein = [0.255227, 1.174503, 2.675816, 4.022211, 6.941683, 9.575530]
fbLevenshtein = [0.549787, 1.901921, 3.791124, 5.911732, 9.231984, 14.285577]
forwardLevenshteinSpace = [0.549787, 1.901921, 3.791124, 5.911732, 9.231984, 14.285577]
backwardLevenshteinSpace = [0.255227, 1.174503, 2.675816, 4.022211, 6.941683, 9.575530]
fbLevenshteinSpace = [0.549787, 1.901921, 3.791124, 5.911732, 9.231984, 14.285577]

forwardLCS = [0.549787, 1.901921, 3.791124, 5.911732, 9.231984, 14.285577]
backwardLCS = [0.255227, 1.174503, 2.675816, 4.022211, 6.941683, 9.575530]
fbLCS = [0.255227, 1.174503, 2.675816, 4.022211, 6.941683, 9.575530]
forwardLCSspace = [0.549787, 1.901921, 3.791124, 5.911732, 9.231984, 14.285577]
backwardLCSspace = [0.255227, 1.174503, 2.675816, 4.022211, 6.941683, 9.575530]
fbLCSspace = [0.255227, 1.174503, 2.675816, 4.022211, 6.941683, 9.575530]

diagonalLevenshtein = [0.549787, 1.901921, 3.791124, 5.911732, 9.231984, 14.285577]
diagonalLCS = [0.255227, 1.174503, 2.675816, 4.022211, 6.941683, 9.575530]

plt.figure(1)
plt.plot(x, list(zip(forwardLevenshtein, backwardLevenshtein)), label=['forward', 'backward'])
plt.title('Forward vs Backward Levenshtein comparison')
plt.xlabel('string length')
plt.ylabel('average execution time (seconds)')
plt.legend()


plt.figure(2)
plt.plot(x, list(zip(forwardLCS, forwardLCSspace)), label=['forward', 'forward with space optimization'])
plt.title('Forward LCS vs Forward LCS with space optimization')
plt.xlabel('string length')
plt.ylabel('average execution time (seconds)')
plt.legend()


plt.figure(3)
plt.plot(x, list(zip(forwardLevenshtein, fbLevenshtein)), label=['forward', 'forward-backward'])
plt.title('Forward vs Forward-backward Levenshtein comparison')
plt.xlabel('string length')
plt.ylabel('average execution time (seconds)')
plt.legend()


plt.figure(4)
plt.plot(x, list(zip(forwardLCS, diagonalLCS)), label=['forward', 'diagonal'])
plt.title('Forward vs Diagonal LCS')
plt.xlabel('string length')
plt.ylabel('average execution time (seconds)')
plt.legend()


plt.show()
