
#include <cstring>
#include <unistd.h>

#define MAX_SIZE 100000000
#define LARGE_ENOUGH 128
#define SIZE_NOT_ALIGNED(size) (size%4!=0)
#define ALIGN_SIZE(size) (4-(size%4))


struct meta_data{
    bool is_free;
    size_t block_size;
    size_t current_size;
    void* start_of_alloc;
    meta_data* next_ptr;
    meta_data* prev_ptr;
};

#define ALIGNED_META_DATA ((sizeof(meta_data)%4==0) ?\
                    sizeof(meta_data) : sizeof(meta_data)+ALIGN_SIZE(sizeof(meta_data)))
meta_data* first_data = NULL;
meta_data* last_data = NULL;

void check_and_split(meta_data* current, size_t size){
    if(current->block_size<(size+ALIGNED_META_DATA+LARGE_ENOUGH))
        return;

    meta_data* new_meta_data=(meta_data*)((current->start_of_alloc)+size);        //inserts new meta_data to list
    new_meta_data->is_free=true;
    new_meta_data->block_size=current->block_size-(size+ALIGNED_META_DATA);
    new_meta_data->current_size=new_meta_data->block_size;

    new_meta_data->next_ptr=current->next_ptr;          //updates ptrs of list
    current->next_ptr->prev_ptr=new_meta_data;
    current->next_ptr=new_meta_data;
    new_meta_data->prev_ptr=current;

}

void check_and_combine(meta_data* to_release){
    if(to_release->prev_ptr!=NULL && to_release->prev_ptr->is_free){        //combine to_release and prev
        to_release->prev_ptr->next_ptr=to_release->next_ptr;

        if(to_release->next_ptr)            //checks that to_release is not the last node in list
            to_release->next_ptr->prev_ptr=to_release->prev_ptr;

        to_release->prev_ptr->block_size+=(ALIGNED_META_DATA+to_release->block_size);
        to_release=to_release->prev_ptr;
    }

    if(to_release->next_ptr!=NULL && to_release->next_ptr->is_free){        //combine to_release and next
        if(to_release->next_ptr->next_ptr)
            to_release->next_ptr->next_ptr->prev_ptr=to_release;

        to_release->block_size+=(ALIGNED_META_DATA+to_release->next_ptr->block_size);

        to_release->next_ptr=to_release->next_ptr->next_ptr;
    }
}

/*
 *   Returns NULL if there is no empty place in the list
 */
meta_data* find_first_fitting_place(size_t size){
    meta_data* current=first_data;
    while(current){
        if(!current->is_free) {                         //if the current block is occupied - continue
            current=current->next_ptr;
            continue;
        }
        if(current->block_size>=size) {
            check_and_split(current,size);
            return current;
        }
        current=current->next_ptr;                      //else - block is free but not big enough
    }

    return NULL;
}
meta_data* find_meta_data_by_user_ptr(void* user_ptr){
    if(user_ptr==NULL)
        return NULL;

    meta_data* current=first_data;
    while(current){
        if(current->start_of_alloc==user_ptr)
            return current;

        current=current->next_ptr;
    }
    return NULL;
}

meta_data* create_new_meta_data(size_t size){
    meta_data* data_to_add=(meta_data*)sbrk(ALIGNED_META_DATA);
    if(data_to_add==(void*)(-1))
        return NULL;

    data_to_add->is_free=true;                         //initializing the mete_data fields
    data_to_add->current_size=size;
    data_to_add->block_size=size;
    data_to_add->start_of_alloc=sbrk(size);

    if(data_to_add->start_of_alloc==(void*)(-1)){       //if allocation failed we dont add data_to_add to the list
        return NULL;
    }

    if(!first_data){
        first_data=data_to_add;
        last_data=data_to_add;
        data_to_add->next_ptr=NULL;
        data_to_add->prev_ptr=NULL;
    }else {
        last_data->next_ptr = data_to_add;
        data_to_add->prev_ptr = last_data;
        data_to_add->next_ptr = NULL;
        last_data = data_to_add;
    }
    return data_to_add;
}



//--------------------------------------------------------------------------------------------------------//
//-----------------------------------------Part 2 Functions-----------------------------------------------//
//--------------------------------------------------------------------------------------------------------//
void* malloc(size_t size){
    if(size == 0 || size > MAX_SIZE)
        return NULL;

    if(SIZE_NOT_ALIGNED(size))
        size+=ALIGN_SIZE(size);


    meta_data* ptr=find_first_fitting_place(size);
    if (ptr!=NULL){                                     //ptr = existing meta data that is currently free
        ptr->current_size=size;
        ptr->is_free=false;
        return ptr->start_of_alloc;
    }
    ptr=create_new_meta_data(size);                     //ptr = new meta_data, inserted last to the list

    if(ptr==NULL)                                       //ptr = NULL, if sbrk doesnt succeed
        return NULL;

    ptr->is_free=false;                                 //mark that allocation succeeded
    return ptr->start_of_alloc;
}


void free(void* p){
    meta_data* to_release=find_meta_data_by_user_ptr(p);
    if(to_release!=NULL) {
        to_release->is_free = true;
        check_and_combine(to_release);
    }
}


void* calloc(size_t num, size_t size){
    void* ptr = malloc(size*num);
    if(ptr==NULL)
        return NULL;

    return std::memset(ptr, 0, size*num);
}

void* realloc(void* oldp, size_t size){
    if(size == 0 || size > MAX_SIZE)
        return NULL;

    meta_data* old_meta_data=find_meta_data_by_user_ptr(oldp);
    if(old_meta_data==NULL)                             //oldp is NULL or there is no meta_data that holds oldp
        return malloc(size);

    if(old_meta_data->block_size>=size){                 //there is enough space in old block for realloction
        old_meta_data->current_size=size;
        return old_meta_data->start_of_alloc;
    }

    void* new_start_of_alloc=malloc(size);
    if(new_start_of_alloc==NULL)                             //if allocation failed we dont free oldp
        return NULL;

    void* tmp_ptr=std::memcpy(new_start_of_alloc, old_meta_data->start_of_alloc, size);
    if(tmp_ptr==NULL)
        return NULL;

    old_meta_data->is_free=true;                        //here we free the old space
    return new_start_of_alloc;
}

//--------------------------------------------------------------------------------------------------------//
//-------------------------------------Underline Functions------------------------------------------------//
//--------------------------------------------------------------------------------------------------------//

size_t _num_free_blocks(){
    meta_data* current=first_data;
    size_t num_free_blocks=0;

    while(current){
        if(current->is_free)
            num_free_blocks++;
        current=current->next_ptr;
    }

    return num_free_blocks;
}

size_t _num_free_bytes(){
    meta_data* current=first_data;
    size_t num_free_bytes=0;

    while(current){
        if(current->is_free)
            num_free_bytes+=current->block_size;

        current=current->next_ptr;
    }

    return num_free_bytes;
}

size_t _num_allocated_blocks(){
    meta_data* current=first_data;
    size_t num_blocks=0;

    while(current){
        num_blocks++;
        current=current->next_ptr;
    }

    return num_blocks;
}

size_t _num_allocated_bytes(){
    meta_data* current=first_data;
    size_t num_allocated_bytes=0;

    while(current){
        num_allocated_bytes+=current->block_size;
        current=current->next_ptr;
    }

    return num_allocated_bytes;
}

size_t _num_meta_data_bytes(){
    return (_num_allocated_blocks() * sizeof(meta_data));
}

size_t _size_meta_data(){
    return sizeof(meta_data);
}

//--------------------------------------------------------------------------------------------------------//
//--------------------------------------------------------------------------------------------------------//
//--------------------------------------------------------------------------------------------------------//
