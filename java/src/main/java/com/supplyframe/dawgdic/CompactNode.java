package com.supplyframe.dawgdic;

public class CompactNode{

  private int compactNodeInt;
  private static final int BITSHIFTS_VALUE = 1;
  private static final int BITSHIFTS_CHILD = 2;
  private static final int BIT_MASK_SIBLING = 1;
  private static final int BIT_MASK_STATE = 2;

  public CompactNode(){
    this.compactNodeInt = 0;
  }

  public void setCompactNodeInt(int compactNodeInt){
    this.compactNodeInt = compactNodeInt;
  }

  public int getCompactNodeInt(){
    return this.compactNodeInt;
  }

  public int getChild(){
    return this.compactNodeInt >> BITSHIFTS_CHILD;
  }

  public boolean hasSibling(){
    return (this.compactNodeInt & BIT_MASK_SIBLING)>0 ? true : false;
  }

  public int getValue(){
    return this.compactNodeInt >> BITSHIFTS_VALUE;
  }

  public boolean isState(){
    return (this.compactNodeInt & BIT_MASK_STATE)>0 ? true : false;
  }

  
}
