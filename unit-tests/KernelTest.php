<?php

/*
  +------------------------------------------------------------------------+
  | Phalcon Framework                                                      |
  +------------------------------------------------------------------------+
  | Copyright (c) 2011-2012 Phalcon Team (http://www.phalconphp.com)       |
  +------------------------------------------------------------------------+
  | This source file is subject to the New BSD License that is bundled     |
  | with this package in the file docs/LICENSE.txt.                        |
  |                                                                        |
  | If you did not receive a copy of the license and are unable to         |
  | obtain it through the world-wide-web, please send an email             |
  | to license@phalconphp.com so we can send you a copy immediately.       |
  +------------------------------------------------------------------------+
  | Authors: Andres Gutierrez <andres@phalconphp.com>                      |
  |          Eduar Carvajal <eduar@phalconphp.com>                         |
  |          ZhuZongXin <dreamsxin@qq.com>                                 |
  +------------------------------------------------------------------------+
*/

class KernelTest extends PHPUnit\Framework\TestCase
{
	public function testNormal()
	{
		$str = Phalcon\Kernel::evalString('Hello <?= "world!" ?>');
		$this->assertEquals($str, "Hello world!");

		$str = Phalcon\Kernel::evalString('Hello <?= $str ?>', array('str' => 'Phalcon7!'));
		$this->assertEquals($str, "Hello Phalcon7!");
	}
}
