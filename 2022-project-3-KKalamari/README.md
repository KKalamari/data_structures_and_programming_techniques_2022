![run-tests](../../workflows/run-tests/badge.svg)

## 2022 Project 3

Εκφώνηση: https://k08.chatzi.org/projects/project3/


### Προσωπικά στοιχεία

__Όνομα__:Κυριακή Καλαμάρη      

### Ασκήσεις που παραδίδονται

Συμπληρώστε εδώ τις ασκήσεις (και τυχόν bonus) που παραδίδετε. Αν κάποια άσκηση
βρίσκεται σε μη αναμενόμενα αρχεία αναφέρετε και τα αρχεία αυτά.

- Άσκηση 1
- Άσκηση 2

### Documentation

Συμπληρώστε εδώ __όσο documentation χρειάζεται__ ώστε οι βαθμολογητές να
κατανοήσουν πλήρως τις λύσεις σας και να τις βαθμολογήσουν ανάλογα. Αυτό θα
πρέπει να γίνει ανεξάρτητα με το αν ο κώδικάς σας είναι καλά σχολιασμένος,
πράγμα που συνιστάται.

Η άσκηση 1 δεν ειναι υλοποιημένη πλήρως.Στο stuck map_node έχω βάλει state , key , value συν ένα 
Pointer array το οποί δέχεται μέχρι fixed_size στοιχεία και στο struct map εχω βάλει και ενα bool full για να ελέγχω πότε πρέπει 
να βαζω τα στοιχεία σε set. Στη map_create αρχικοποιώ empty το state όλου του array του map και 
null όλα τα στοιχεία του pointer array του. Έχω φτιάξει rehash συνάρτηση η οποία γίνεται όταν ο load factor γινεται
μεγαλύτερος από max_load_factor. 

Η άσκηση 2 δεν είναι υλοποιημένη πλήρως. Έχω 2 arrays μέσα στο struct του map και προσπαθω να υλοποιήσω το cuckoo hashing. 
