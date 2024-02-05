import sys

inputstr = sys.argv[1]
tokens = inputstr.split(' ')
operations = ['+', '-']

if inputstr.startswith('-'):
    raise ValueError('First number cannot be negative')
if not tokens[0].isdigit():
    raise ValueError('First token must be a number')

last_token = '+'
result = 0
for token in tokens:
    if not len(token):
        continue
    if last_token in operations and token in operations:
        raise ValueError('Two operators in a row')
    if last_token.isdigit() and token.isdigit():
        raise ValueError('Two numbers in a row')
    if last_token == '+':
        result += int(token)
    elif last_token == '-':
        result -= int(token)
    last_token = token

print(result)