# Derleyici ve bayraklar
CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -D_XOPEN_SOURCE=700

# Derlenecek programlar
TARGETS = hogwarts wizard professor

# Tüm programları derlemek için varsayılan kural
all: $(TARGETS)

# Sunucu (hogwarts) için bağlama (link) kuralı
hogwarts: hogwarts.o defs.o
	$(CC) $(CFLAGS) -o $@ $^

# Büyücü (wizard) istemcisi için bağlama kuralı
wizard: wizard.o defs.o
	$(CC) $(CFLAGS) -o $@ $^

# Profesör (professor) istemcisi için bağlama kuralı
professor: professor.o defs.o
	$(CC) $(CFLAGS) -o $@ $^

# .c dosyalarından .o (object) dosyaları oluşturma kuralı
%.o: %.c defs.h
	$(CC) $(CFLAGS) -c $< -o $@

# Derlenmiş dosyaları temizleme kuralı
clean:
	rm -f *.o $(TARGETS)

.PHONY: all clean