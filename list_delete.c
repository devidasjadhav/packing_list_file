#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>


struct node_s {
    struct node_s * next;
    struct node_s * prev;
};

struct emp_s {
	struct node_s node;
	uint32_t eid;
	uint8_t name[20];
	uint8_t exp;
};



typedef int (* cmp_user_data_var)(struct node_s * , struct node_s *);


#define CONTAINER_OF(node_ptr,type,member) ((type *)(node_ptr - offsetof(type , member)))
struct node_user_data * head_node = NULL;
struct node_user_data * root_node = NULL;
int node_count = 0;

int cmp_user_data_func(struct node_s * node1 , struct node_s * node2){

    return ((CONTAINER_OF(node1,struct emp_s,node)->eid) - (CONTAINER_OF(node2,struct emp_s,node)->eid));
}

int insert_node(struct node_s * new_node, struct node_s ** head , cmp_user_data_var cmp_user_data){
    struct node_s * node_last;
    int ret;
    if(*head == NULL){
        *head = new_node;
        (*head)->next = NULL;
        (*head)->prev = NULL;
        return 0;
    }
    for(struct node_s * node = *head; node != NULL ; node = node->next){
        ret = cmp_user_data(node,new_node);
        if(ret >= 0){
            if(node->prev == NULL){
                *head = new_node;
            }
            else {
                 node->prev->next = new_node;
            }
            new_node->prev = node->prev;
            node->prev = new_node;
            new_node->next = node;
            return ret;
        }
        else {
            node_last = node;
        }
    }
    node_last->next = new_node;
    new_node->prev = node_last;
    new_node->next = NULL;
    return -1;
}

int remove_node(struct node_s * del_node ,struct node_s ** head , int (*cmp_user_data)(struct node_s * , struct node_s *)){
    char ret;
    if(*head == NULL){
        return 0;
    }
    // delete if head is same as del_node but you have to iterate over to get that node
    for(struct node_s * node = *head; node != NULL ; node = node->next){
        ret = cmp_user_data(del_node,node);
        if(ret == 0){
            if(node->prev == NULL){
                del_node = node;
                if(node->next == NULL){
                    return -3;
                }
                *head = node->next;
                node->next->prev = NULL;
                return -2;
            }
            node->prev->next = node->next;
            node->next->prev = node->prev;
            del_node = node;
            //asuming del_node is not needed to be freed
        }
    }
    del_node = NULL;
    return -1;
}

int insert_node_to_left(struct node_s * new_node, struct node_s ** head , struct node_s * node){
	struct node_s * node_temp;
	if(new_node == NULL){
		return -1;
		//handle this case
	}
	if(node == NULL){
		*head = node_temp;
		new_node->prev = NULL;
		new_node->next = NULL;
		return 0;
	}
	node_temp = node->prev;
	if(node_temp == NULL){
		if(node != *head){
			return -2;
		}
		*head = new_node;
		new_node->prev = NULL;
		new_node->next = node;
		return 0;
	}

	node->prev = new_node;
	new_node->next = node;
	new_node->prev = node_temp;
	node_temp->next = new_node;
	return 1;
}

int insert_node_to_right(struct node_s * new_node, struct node_s ** head , struct node_s * node){
	struct node_s * node_temp;
	if(new_node == NULL){
		return -1;
		//handle this case
	}
	if(node == NULL){
		*head = node_temp;
		new_node->prev = NULL;
		new_node->next = NULL;
		return 0;
	}
	node_temp = node->next;

	node->next = new_node;
	new_node->prev = node;
	new_node->next = node_temp;
	if(node_temp != NULL){
		node_temp->prev = new_node;
	}
	return 1;
}

int delete_intermediate_node(struct node_s ** head , struct node_s * node){
	struct node_s *del_node, *prev_delnode,*next_delnode;
	if(node == NULL){
		return -1;
	}
	if(node == *head){
		*head == NULL;
		return 0;
	}
	prev_delnode = node->prev;
	next_delnode = node->next;
	if(prev_delnode == NULL){
		return -3;
		//very unlikely as head will have this property
	}
	prev_delnode->next = next_delnode;
	next_delnode->prev = prev_delnode;
	return 1;
}

struct node_s * where_to_insert(struct node_s * base_node ,struct node_s * new_node , char left_right , cmp_user_data_var cmp_user_data_func){
	
	struct node_s *loop_node, *prev_node = NULL;
	
	if((base_node == NULL)||(new_node == NULL)){
		return NULL;
	}

	for(loop_node = base_node; loop_node != NULL; loop_node = ((left_right == 0) ? loop_node->prev : loop_node->next)){
		
		if(!(cmp_user_data_func(loop_node,new_node))){
			return loop_node;
		}
		
		if(loop_node != NULL){
			prev_node = loop_node;
		}
	}
/*
	if( prev_node == NULL){
		return NULL;
	}
*/
	return prev_node;	
}


int iterate_list( struct node_s * head){
	struct emp_s *temp;
    for(struct node_s * node = head; node != NULL ; node = node->next){
        temp = CONTAINER_OF(node,struct emp_s ,node); 
        printf("EID: %d\t NAME: %20s\t EXP: %d\n",temp->eid,temp->name,temp->exp); 
    }
    return 0;
}

struct node_s * create_node(uint32_t eid,uint8_t *name, uint8_t exp,struct node_s **head){
    struct emp_s * temp_user_node = malloc(sizeof(struct emp_s));
    //TODO code for return failure
    temp_user_node->eid = eid;
    temp_user_node->exp = exp;
    strcpy(temp_user_node->name , name);
    insert_node(&(temp_user_node->node),head,cmp_user_data_func);
    return (&temp_user_node->node);
}


/******************************
 *
 *
 ******************************/


int packer_emp(struct emp_s *emp_ptr, uint8_t * buffer){
	uint8_t * buffer_bkp = buffer;
	memcpy(buffer,&(emp_ptr->eid),sizeof(emp_ptr->eid));
	buffer += sizeof(emp_ptr->eid);
	memcpy(buffer,&(emp_ptr->exp),sizeof(emp_ptr->exp));
	buffer += sizeof(emp_ptr->exp);
	memcpy(buffer,emp_ptr->name,20);

	buffer += 20;
	return (buffer - buffer_bkp);
}

int unpacker_emp(struct emp_s *emp_ptr, uint8_t * buffer){ 
	uint8_t * buffer_bkp = buffer;
	memcpy(&(emp_ptr->eid),buffer,sizeof(emp_ptr->eid));
	buffer += sizeof(emp_ptr->eid);
	memcpy(&(emp_ptr->exp),buffer,sizeof(emp_ptr->exp));
	buffer += sizeof(emp_ptr->exp);
	memcpy(emp_ptr->name,buffer,20);

	buffer += 20;
	return (buffer - buffer_bkp);
}

int iterate_list_pack( uint64_t fd, struct node_s * head){
	struct emp_s *temp, temp_unpack;
	uint8_t writen_bytes ,buffer[60];
    for(struct node_s * node = head; node != NULL ; node = node->next){
        temp = CONTAINER_OF(node,struct emp_s ,node); 
        packer_emp(temp,buffer);
	writen_bytes = write(fd,buffer,sizeof(buffer));
	//check writen_bytes
	unpacker_emp(&temp_unpack,buffer);
	printf("EID: %d\t NAME: %20s\t EXP: %d\n",temp_unpack.eid,temp_unpack.name,temp_unpack.exp); 
	//printf("EID: %d\t NAME: %20s\t EXP: %d\n",temp->eid,temp->name,temp->exp); 
    }
    return 0;
}

int read_from_file( uint64_t fd){
	struct emp_s temp_unpack;
	uint8_t ret = 1,number = 0, buffer[60];
	while(ret > 0){
		number ++;
		ret = read(fd,buffer,60);
		if(ret == 0)
			break;
		unpacker_emp(&temp_unpack,buffer);
		printf(" %d EID: %d\t NAME: %20s\t EXP: %d\n", ret ,temp_unpack.eid,temp_unpack.name,temp_unpack.exp); 
	}
	return number;
}	



int main(){
    struct node_s * head;
    struct node_s * nnode;
    uint32_t fd;
    head = NULL;
    create_node(88472,"Bhanuprakash",14,&head);
    create_node(20565,"HariPriya",3,&head);
    create_node(23435,"Devidas",6,&head);

    iterate_list(head);
    
    fd = open("./dummy_file", O_WRONLY | O_CREAT);

    iterate_list_pack(fd, head);

    close(fd);

    
    for(struct node_s * node = head; node != NULL ; ){	
        printf("test\n");
        nnode = node->next;
        remove_node(node,&head,cmp_user_data_func);
        free(CONTAINER_OF(node,struct emp_s,node));
        node = nnode;
    }

    fd = open("./dummy_file", O_RDONLY | O_CREAT);

    read_from_file(fd);
    

    close(fd);

    return 0;
}
