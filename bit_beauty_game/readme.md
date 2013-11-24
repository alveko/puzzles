Problem:
-------- 

The beauty of a number X is the number of 1s in the binary representation of X.

Two players are plaing a game. There is number N written on a black board.

The game is played as following:
Each time a player chooses an integer number (0 <= k) so that 2^k is less than N
and (N-2^k) is as beautiful as N.
Next he removes N from blackboard and writes N-2^k instead.

The player that can not continue the game (there is no such k that satisfies
the constrains) looses the game.

The First player starts the game and they play in turns alternatively.
Knowing that both two players play optimally you have to specify the winner.

Input:
First line of the Input contains an integer T, the number of testcase. 0<=T<=5.
Then follow T lines, each containing an integer N. 0<N<=10^9+1

Output:
For each testcase print "First Player" if first player can win the game and
"Second Player" otherwise.

Sample Input:
7
1
2
8
16
42
1000
123

Sample Output:
Second Player
First Player
First Player
Second Player
Second Player
First Player
Second Player

Explanation:
In the first example N is 1 and first player can't change it so the winner is
the second player.

In the second example N is 2, so the first player subtracts 1 (2^0) from N and
the second player looses the game.
