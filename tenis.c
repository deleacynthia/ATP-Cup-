#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

//Structura pentru jucator:
typedef struct Player { 
   char *last_name;
   char *first_name;
   int score;
} Player;
 
//Structura pentru tara:
typedef struct Country {
   char *name;
   int nr_players;
   int global_score;
   Player *players;
} Country;

// Structura pentru lista dublu inlantuita cu santinela:
typedef struct node {
	Country *country;
	struct node *next;
	struct node *prev;
} node_t;

// Structura pentru stiva:
typedef struct stack {
	Country *Country;
	struct stack *next;
} stack_t;

// Structura pentru coada:
typedef struct queue {
	int first;			// pozitia in vector a primului jucator
	int second;			// pozitia in vector a celui de-al doilea jucator
	struct queue *next;
} queue_t;

// Functie care intoarce adresa unei noi liste, unde primul nod este santinela
node_t *init() {
	node_t *list = malloc(sizeof(node_t));
	if (list == NULL) {
		free(list);
		return NULL;
	}
	list->next = list->prev = list;
	list->country = NULL;

	return list;
}

// Adauga un nou element la capatul listei
void insert_list(node_t *l, Country *c) {
	// Iau ultimul element din lista
	node_t *last = l->prev;

	// Aloc memorie pentru un nou element
	node_t *new_node = malloc(sizeof(node_t));
	if (new_node == NULL) {
		free(new_node);
		return;
	}

	// Fac legaturile
	new_node->next = l;
	new_node->prev = last;
	last->next = new_node;
	l->prev = new_node;

	// Adaug si valoarea potrivita
	new_node->country = c;
}

// Functie auxiliara care dezaloca memoria pentru o tara
void delete_country(Country *c) {
	int i;
	// Dezaloc numele si prenumele jucatorilor
	for (i = 0; i < c->nr_players; ++i) {
		free(c->players[i].first_name);
		free(c->players[i].last_name);
	}
	// Dezaloc vectorul de jucatori
	free(c->players);				
	// Dezaloc numele tarii
	free(c->name);
	// Dezaloc tara
	free(c);

	c = NULL;
}

// Functie care sterge un nod din lista
void delete_node(node_t *n) {
	// Iau primul element din lista
	node_t *next = n->next;
	node_t *prev = n->prev;

	// Refac legaturile
	next->prev = prev;
	prev->next = next;

	// Dezaloc tara
	delete_country(n->country);

	// Dezaloc nodul
	free(n);
}

// Functie care elibereaza memoria alocata pentru o lista
void delete_list(node_t *l) {
	// Iau primul element din lista
	node_t *first = l->next;
	node_t *aux;

	// Parcurg lista si dezaloc fiecare nod
	while (first != l) {
		aux = first;
		first = first->next;
		
		// Dezaloc tara
		delete_country(aux->country);

		// Dezaloc nodul
		free(aux);
	}
	// Dezaloc santinela
	free(first);
}

// Functie care afiseaza toate numele tarilor
void print_list(node_t *l, FILE *out) {
	// Iau primul element din lista
	node_t *first = l->next;

	// Parcurg lista si afisez numele fiecarei tari
	while (first != l) {
		fprintf(out, "%s\n", first->country->name);
		first = first->next;
	}
}

// Functie care citeste toate datele din fisier si populeaza lista de tari
int read_data(node_t *l, char *file_name) {
	int n, i, j;
	char f_name[15], l_name[15], country[15];
	FILE *in = fopen(file_name, "r");

	// Citesc numarul de tari
	fscanf(in, "%d", &n);
	for (i = 0; i < n; ++i) {
		// Copiez numele tarii in structura
		Country *c = malloc(sizeof(Country));
		if (c == NULL) {
			return 0;
		}

		// Citesc numarul de jucatori
		fscanf(in, "%d\n", &(c->nr_players));

		// Curat bufferul
		memset(country, 0, sizeof(country));
		// Citesc numele tarii
		fscanf(in, "%s", country);
		
		c->name = calloc(sizeof(char), strlen(country) + 1);
		if (c->name == NULL) {
			return 0;
		}
		memcpy(c->name, country, strlen(country));
		
		// Aloc vectorul de jucatori
		c->players = malloc(sizeof(Player) * c->nr_players);
		if (c->players == NULL) {
			return 0;
		}

		// Initializez scorul global cu 0 ca sa il pot calcula
		c->global_score = 0;

		for (j = 0; j < c->nr_players; ++j) {
			// Curat bufferele
			memset(f_name, 0, sizeof(f_name));
			memset(l_name, 0, sizeof(l_name));
			fscanf(in, "%s %s %d", f_name, l_name, &(c->players[j].score));

			// Adaug scorul jucatorului la scorul global
			c->global_score += c->players[j].score;

			// Adaug numele jucatorilor
			c->players[j].first_name = calloc(sizeof(char), strlen(f_name) + 1);
			strcpy(c->players[j].first_name, f_name);

			c->players[j].last_name = calloc(sizeof(char), strlen(l_name) + 1);
			strcpy(c->players[j].last_name, l_name);
		}
		
		// Adaug tara in lista
		insert_list(l, c);
	}

	// Inchid fisierul
	fclose(in);

	return n;
}

// Functie care reduce numarul de tari pana la o putere a lui 2
void eliminate_countries(node_t *l, int *n) {
	// Cat timp numarul de tari nu este putere al lui 2,
	// scot din lista cate o tara

	while(ceil(log2(*n)) != floor(log2(*n))) {
		// Determin tara cu cel mai mic scor
		node_t *aux;

		// Presupun ca primul element din lista este cel cu
		// scorul cel mai mic
		int min = l->next->country->global_score;

		// Iau primul element din lista
		node_t *first = l->next;
		aux = first;

		// Parcurg lista si determin scorul minim
		while (first != l) {
			if (min > first->country->global_score) {
				min = first->country->global_score;
				aux = first;
			}

			first = first->next;
		}

		// Sterg nodul din lista
		delete_node(aux);

		// Scad numarul de tari
		*n = *n - 1;
	}
}

// Functia intoarce un nod nou pentru stiva, cu tara adaugata 
stack_t *new_stack_node(Country *c) {
	stack_t *new_node = malloc(sizeof(stack_t));
	if (new_node == NULL) {
		free(new_node);
		return NULL;
	}
	new_node->Country = c;
	new_node->next = NULL;

	return new_node;
}

// Functie care adauga un element in stiva
void push(stack_t **s, Country *c) {
	stack_t *new_node = new_stack_node(c);
	if (new_node == NULL) {
		return;
	}

	new_node->next = *s;
	*s = new_node;
}

// Functie care scoate un element din stiva si ii intoarce valoarea
Country *pop(stack_t **s) {
	if (*s == NULL) {
		return NULL;
	}

	// Retin valoarea tarii
	Country *aux_c = (*s)->Country;
	
	// Elimin primul element din stiva
	stack_t *aux_s = *s;
	*s = (*s)->next;
	
	// Dezaloc memoria alocata pentru nod
	free(aux_s);

	// Intorc valoarea
	return aux_c;
}

// Functie care sterge o stiva
void destroy_stack(stack_t **s) {
	while (*s) {
		pop(s);
	}
	free(*s);
	*s = NULL;
}

// Functie care intoarce numarul de elemente din stiva
int get_size_stack(stack_t *s) {
	if (s == 0) {
		return 0;
	}
	stack_t *aux = s;
	int cnt = 0;

	while (aux) {
		aux = aux->next;
		cnt++;
	}

	return cnt;
}

// Functia genereaza o stiva cu toate tarile participante la concurs din lista
// folosita la punctele 1 si 2
stack_t *generate_stack(node_t *list) {
	stack_t *new_stack = NULL;

	// Iau primul element din lista
	node_t *first = list->next;

	// Parcurg lista si afisez numele fiecarei tari
	while (first != list) {
		// Resetez scorul global inainte sa inceapa turneul
		first->country->global_score = 0;
		
		// Adaug tara in stiva
		push(&new_stack, first->country);
		first = first->next;
	}

	// Intoarce noua stiva
	return new_stack;
}

// Asemanator cu new_stack_node intoarce un nou nod pentru coada
queue_t *new_queue_node(int p1, int p2) {
	queue_t *new_node = malloc(sizeof(queue_t));
	if (new_node == NULL) {
		free(new_node);
		return NULL;
	}
	new_node->first = p1;
	new_node->second = p2;
	new_node->next = NULL;

	return new_node;
}

// Adauga un nou element in coada
void q_insert(queue_t **queue, int p1, int p2) {
	queue_t *new_node = new_queue_node(p1, p2);
	if (new_node == NULL) {
		return;
	}

	if (*queue == NULL) {
		*queue = new_node;
		return;
	}

	queue_t *aux = *queue;
	while(aux->next != NULL) {
		aux = aux->next;
	}

	aux->next = new_node;
}

// Scoate un element din coada si-l returneaza in variabilele p1 si p2
void q_delete(queue_t **queue, int *p1, int *p2 ) {
	queue_t *q_aux = *queue;

	if (p1 != NULL && p2 != NULL) {
		*p1 = (*queue)->first;
		*p2 = (*queue)->second;
	}

	*queue = (*queue)->next;
	free(q_aux);
}

// Functia joaca o etapa a turneului si intoarce stiva de castigatori
stack_t *play_round(int round, stack_t **stack, FILE *out) {
	int i, j;
	stack_t *winner = NULL;

	fprintf(out, "\n====== ETAPA %d ======\n", round);

	while (*stack) {
		// Extrag 2 tari din stiva
		Country *first_c = pop(stack);
		Country *second_c = pop(stack);
		int score_first = 0, score_second = 0;
		queue_t *games = NULL;

		// Adaug meciurile in coada
		for (i = 0; i < first_c->nr_players; ++i) {
			for (j = 0; j < second_c->nr_players; ++j) {
				q_insert(&games, i, j);
			}
		}

		// Afisez cine joaca
		fprintf(out, "\n%s %d ----- ", first_c->name, first_c->global_score);
		fprintf(out, "%s %d\n", second_c->name, second_c->global_score);

		// Incep meciurile
		while (games != NULL) {
			int p1, p2;

			// Extrag meciul
			q_delete(&games, &p1, &p2);
			
			// Afisez cine joaca
			fprintf(out, "%s %s %d vs ",
				first_c->players[p1].first_name,
				first_c->players[p1].last_name,
				first_c->players[p1].score);
			fprintf(out, "%s %s %d\n", 
				second_c->players[p2].first_name,
				second_c->players[p2].last_name,
				second_c->players[p2].score);

			// Verific cine a castigat si adaug scorul
			if (first_c->players[p1].score > second_c->players[p2].score) {
				// Jucatorul 1 castiga
				first_c->players[p1].score += 5;
				score_first += 3;
			} else if (first_c->players[p1].score < second_c->players[p2].score) {
				// Jucatorul 2 castiga
				second_c->players[p2].score += 5;
				score_second += 3;
			} else {
				// Egalitate
				first_c->players[p1].score += 2;
				second_c->players[p2].score += 2;
				score_first += 1;
				score_second += 1;
			}
		}

		// Cand se termina meciurile verific cine a castigat, adaug tara
		// respectiva in stiva de invingatori si ii schimb scorul
		if (score_first > score_second) {
			// Tara 1 castiga
			first_c->global_score += score_first;
			push(&winner, first_c);
		} else if (score_first < score_second) {
			// Tara 2 castiga
			second_c->global_score += score_second;
			push(&winner, second_c);
		} else {
			// Egalitate
			
			// Calculez scorul cel mai mare dintre jucatorii fiecarei tari
			int score_max_1 = first_c->players[0].score;
			int score_max_2 = second_c->players[0].score;
			for (i = 1; i < first_c->nr_players; ++i) {
				if (first_c->players[i].score > score_max_1) {
					score_max_1 = first_c->players[i].score;
				}
			}
			for (j = 1; j < second_c->nr_players; ++j) {
				if (second_c->players[j].score > score_max_2) {
					score_max_2 = second_c->players[j].score;
				}
			}

			if (score_max_1 < score_max_2) {
				second_c->global_score += score_second;
				push(&winner, second_c);
			} else {
				first_c->global_score += score_first;
				push(&winner, first_c);
			}
		}
	}

	return winner;
}

int main(int argc, char *argv[]) {
	int ex_1, ex_2, ex_3, ex_4, ex_5;
	ex_1 = ex_2 = ex_3 = ex_4 = ex_5 = 0;

	// Verific daca am primit parametrii bine
	if (argc != 4) {
		printf("Numar gresit de parametrii!\n");
		printf("Se ruleaza ./tenis cerinte.in date.in rezultate.out\n");

		return -1;
	}

	// Citesc din fiserul "cerinte.in" ce cerinte rezolv si inchid fisierul
	FILE *in = fopen(argv[1], "r");
	fscanf(in, "%d %d %d %d %d", &ex_1, &ex_2, &ex_3, &ex_4, &ex_5);
	fclose(in);

	// Initializez lista
	node_t *list = init();

	// Numarul de tari
	int n;

	// Deschid fisierul in care scriu
	FILE *out = fopen(argv[3], "w");

	if (ex_1) {
		// Citesec datele de intrare
		n = read_data(list, argv[2]);
	}

	if (ex_2) {
		// Elimin tari
		eliminate_countries(list, &n);
	}
	// Afisez tarile
	print_list(list, out);

	if (ex_3) {
		// Generez stiva
		stack_t *countries = generate_stack(list);
		stack_t *winners = NULL;
		int round = 1;

		// Joc prima runda by default
		winners = play_round(round, &countries, out);
		// Joc pana cand ramane o singura tara invingatoare

		while (get_size_stack(winners) != 1) {
			// Sterg vechea stiva
			// destroy_stack(&countries);

			// Afisez castigatorii etapei anterioare
			fprintf(out, "\n=== WINNER ===\n");
			
			while (winners != NULL) {
				// Scot din stiva tara castigatoare
				Country *c = pop(&winners);
				// Afisez lista
				fprintf(out, "%s --- %d\n", c->name, c->global_score);
				// O inapoi in noua lista de participanti
				push(&countries, c);
			}
			
			round++;

			winners = play_round(round, &countries, out);
		}

		// Afisez castigatorii etapei anterioare
		fprintf(out, "\n=== WINNER ===\n");
		Country *c = pop(&winners);
		fprintf(out, "%s --- %d\n", c->name, c->global_score);
		
	}

	// Inchid fisierul
	fclose(out);

	// Eliberez memoria alocata listei
	delete_list(list);
	return 0;
}
