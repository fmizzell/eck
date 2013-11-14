<?php

namespace ECK\Behaviors;

class Created{
  
  public static function entityInsert($args){
    return time();
  }
}
