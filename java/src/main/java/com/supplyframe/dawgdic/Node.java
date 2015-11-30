package com.supplyframe.dawgdic;

public class Node{

  private int child;
  private int sibling;
  private char label;
  private boolean isState;
  private boolean hasSibling;
  private static final char EOL='\n';

  public Node(){
    this.child = 0;
    this.sibling = 0;
    this.label = EOL;
    this.isState = false;
    this.hasSibling = false;
  }

  public void setChild(int child){
    this.child = child;
  }

  public int getChild(){
    return this.child;
  }

  public void setSibling(int sibling){
    this.sibling = sibling;
  }

  public int getSibling(){
    return this.sibling;
  }

  public void setLabel(char label){
    this.label = label;
  }

  public char getLabel(){
    return this.label;
  }

  public void setIsState(boolean isState){
    this.isState = isState;
  }

  public boolean getIsState(){
    return this.isState;
  }

  public void setHasSibling(boolean hasSibling){
    this.hasSibling = hasSibling;
  }

  public boolean getHasSibling(){
    return this.hasSibling;
  }

  public int getCompactNodeInt(){
    if(label==EOL){
      return (this.child<<1) | (hasSibling?1:0);
    }else{
      return (this.child<<2) | (isState?2:0) | (hasSibling?1:0);
    }
  }
}
