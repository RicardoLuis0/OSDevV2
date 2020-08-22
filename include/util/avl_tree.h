#ifndef AVL_TREE_H_INCLUDED
#define AVL_TREE_H_INCLUDED

#include "util/tmp.h"
#include "util/smart_ptr.h"

namespace Util {
    template<typename T,typename F=T,auto VALUE_COMPARE_LT=nullptr,auto VALUE_REMOVE=nullptr,auto VALUE_FIND_COMPARE_LT_RIGHT=VALUE_COMPARE_LT,auto VALUE_FIND_COMPARE_LT_LEFT=VALUE_COMPARE_LT>
    class AVLTree {
        
        static bool compare_lt(T &a,T &b){
            if constexpr(TMP::is_null_pointer_dv<VALUE_COMPARE_LT>){
                return (a<b);
            }else{
                return VALUE_COMPARE_LT(a,b);
            }
        }
        
        static bool compare_find_lt_right(T &a,F &b){
            if constexpr(TMP::is_null_pointer_dv<VALUE_FIND_COMPARE_LT_RIGHT>){
                return (a<b);
            }else{
                return VALUE_FIND_COMPARE_LT_RIGHT(a,b);
            }
        }
        
        static bool compare_find_lt_left(F &a,T &b){
            if constexpr(TMP::is_null_pointer_dv<VALUE_FIND_COMPARE_LT_LEFT>){
                return (a<b);
            }else{
                return VALUE_FIND_COMPARE_LT_LEFT(a,b);
            }
        }
        
        struct Node {
            
            Node(const T &v2):v(v2),height(1){//copy
            }
            
            Node(T && v2):v(TMP::move(v2)),height(1){//move
            }
            
            Node(const Node &other):v(other.v),height(1){
            }
            
            Node(Node && other):v(TMP::move(other.v)),height(1){
            }
            
            ~Node(){
                if constexpr(!TMP::is_null_pointer_dv<VALUE_REMOVE>){
                    VALUE_REMOVE(v);
                }
            }
            
            T v;
            UniquePtr<Node> left;
            UniquePtr<Node> right;
            int32_t height;
            
            void update_height(){
                height=max(left?left->height:0,right?right->height:0)+1;
            }
            
            static int32_t calc_balance(UniquePtr<Node> & n){
                return n?(n->left?n->left->height:0)-(n->right?n->right->height:0):0;
            }
            
            static void rotateLL(UniquePtr<Node> & node) {
                //release
                Node * y=node.release();
                Node * x=y->left.release();
                Node * t=x->right.release();
                //rotate
                x->right=y;
                y->left=t;
                //update heights
                x->update_height();
                y->update_height();
                node=x;
            }
            
            static void rotateRR(UniquePtr<Node> & node) {
                //release
                Node * x=node.release();
                Node * y=x->right.release();
                Node * t=y->left.release();
                //rotate
                y->left=x;
                x->right=t;
                //update heights
                x->update_height();
                y->update_height();
                node=y;
            }
            
            static void rotateLR(UniquePtr<Node> & node){
                rotateRR(node->left);
                rotateLL(node);
            }
            
            static void rotateRL(UniquePtr<Node> & node){
                rotateLL(node->right);
                rotateRR(node);
            }
            
            static T * find(Node * root,const F &data){
                while(root){
                    if(compare_find_lt_right(root->v,data)){//v<data
                        root=root->right;
                    }else if(compare_find_lt_left(data,root->v)){//v>data
                        root=root->left;
                    }else{//v==data
                        return &root->v;
                    }
                }
                return nullptr;//couldn't find
            }
            static bool insert(UniquePtr<Node> & p,Node * n){
                if(!p){
                    p=n;
                    return true;
                }
                if(compare_lt(p->v,n->v)){//if n>p
                    if(!insert(p->right,n)){
                        return false;//if couldn't insert node, don't balance
                    }
                }else if(compare_lt(n->v,p->v)){//if n<p
                    if(!insert(p->left,n)){
                        return false;//if couldn't insert node, don't balance
                    }
                }else{//if n==p
                    return false;//duplicate values not allowed
                }
                p->update_height();
                int32_t bal=calc_balance(p);
                if(bal>1){
                    if(compare_lt(n->v,p->left->v)){//n<left
                        rotateRR(p);
                    }else if(compare_lt(p->left->v,n->v)){//n>left
                        rotateRL(p);
                    }
                }else if(bal<-1){
                    if(compare_lt(n->v,p->right->v)){//n<right
                        rotateLR(p);
                    }else if(compare_lt(p->right->v,n->v)){//n>right
                        rotateLL(p);
                    }
                }
                return true;
            }
        };
        UniquePtr<Node> root;
    public:
        
        AVLTree(){
        }
        
        T * find(const F &data){
            return Node::find(root,data);
        }
        
        bool insert(const T &data){
            Node * n=new Node(data);
            if(!insert(root,n)){
                delete n;
                return false;
            }
            return true;
        }
        
        bool insert(T && data){
            Node * n=new Node(data);
            if(!insert(root,n)){
                data=TMP::move(n->v);
                delete n;
                return false;
            }
            return true;
        }
        
    };
}

#endif // AVL_TREE_H_INCLUDED
