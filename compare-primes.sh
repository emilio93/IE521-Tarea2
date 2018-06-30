if [ ! -f ./primes1.zip ]; then
    echo "File not found!"
    wget -O ./primes1.zip 'https://primes.utm.edu/lists/small/millions/primes1.zip' --no-check-certificate
fi

if [ ! -f ./primes1.txt ]; then
    unzip ./primes1.zip -d ./
fi

rm ./output.csv 2>/dev/null

cat primes1.txt | sed -e '/                 The First 1,000,000 Primes (from primes.utm.edu)/d' | sed -E 's/\s+/\n/g' | sed -e '/^\s*$/d' > primes2.txt
head -n 500000 primes2.txt > primes-src.csv

make
make run

sort --numeric-sort primos.csv > primos.sorted.csv
rm primos.csv
head -500000 primos.sorted.csv > primos.csv
rm primos.sorted.csv

paste -d',' primos.csv  <(awk -F',' '{print $NF}' primes-src.csv) > output.csv

rm ./primes1.txt 2>/dev/null
rm ./primes2.txt 2>/dev/null
rm ./primes-src.csv 2>/dev/null
