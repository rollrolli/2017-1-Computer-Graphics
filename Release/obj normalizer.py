
ratio = 3.0
FILENAME = 'ironman.obj'

x = list()
y = list()
z = list()
line = list()

with open(FILENAME, 'r') as f:

	raw = f.readlines()

for l in raw:

	line.append(l)
	l = l.split(' ')

	if l[0] == 'v':

		x.append(float(l[1]) * ratio)
		y.append(float(l[2]) * ratio)
		z.append(float(l[3]) * ratio)

meanx = sum(x) / float(len(x))
meany = sum(y) / float(len(y))
meanz = sum(z) / float(len(z))

translatex = (max(x) + min(x)) / ratio
translatey = (max(y) + min(y)) / ratio
translatez = (max(z) + min(z)) / ratio

i = 0
with open('modified ' + FILENAME, 'w') as f:

	for l in line:

		if l[0] == 'v' and l[1] == ' ':
			
			l = l.split(' ')
			temp = l[0] + ' ' + str((x[i] - min(x))) + ' ' + str((y[i] - min(y))) + ' ' + str((z[i] - min(z))) + '\n'
			f.write(temp)
			i = i + 1

		else:

			f.write(l)

