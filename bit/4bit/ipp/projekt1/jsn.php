<?php

class File
{
	public function Read($fileName)
	{	// reading from file
		$content = file_get_contents($fileName);
		// if reading has failed, exit with error code 2
		if($content === FALSE)
		{
			forceExit(2, "Reading from input file has failed!");
		}

		else
		{	// return the content of input file or stdin
			return $content;
		}
	}

	public function Write($fileName, $content)
	{
		// if writing to file has failed, exit with error code 3
		if(file_put_contents($fileName, $content) === FALSE)
		{
			forceExit(3, "Unable to write to output file!");
		}
	}
}

class Arguments
{
	public static
		$string  = FALSE,	// transforming of strings
		$number  = FALSE,	// transforming of numbers
		$literal = FALSE,	// transforming of literals
		$convert = FALSE,	// converting of problematic chars
		$size    = FALSE,	// additional attribute size
		$index   = FALSE,	// indexing
		$types   = FALSE,	// additional attribute type
		$padding = FALSE;	// padding
	
	private static
		$generate = TRUE, 	// header generating
		$help    = FALSE, 	// help printing
		$inFile  = NULL,	// input file name
		$outFile = NULL,	// output file name
		$subst   = NULL,	// substitution
		$root    = NULL,	// root name
		$array   = NULL,	// array name
		$item    = NULL,	// item name
		$start   = NULL;	// starting index
	// return header, implicit: nothing
	public function GetHeader()
	{
		if(self::$generate)
		{
			return "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
		}
		else
		{
			return '';
		}
	}
	// return input file name, implicit: stdin
	public function GetInputFile()
	{
		if(is_null(self::$inFile))
		{
			return 'php://stdin';
		}
		else
		{
			return self::$inFile;
		}
	}
	// return output file name, implicit: stdout
	public function GetOutputFile()
	{
		if(is_null(self::$outFile))
		{
			return 'php://stdout';
		}
		else
		{
			return self::$outFile;
		}
	}
	// return substitution, implicit: -
	public function GetSubstitution()
	{
		if(is_null(self::$subst))
		{
			return '-';
		}
		else
		{
			return self::$subst;
		}
	}
	// return root name
	public function GetRootName()
	{
		return self::$root;
	}
	// return array name, implicit: array
	public function GetArrayName()
	{
		if(is_null(self::$array))
		{
			return 'array';
		}
		else
		{
			return self::$array;
		}
	}
	// return item name, implicit: item
	public function GetItemName()
	{
		if(is_null(self::$item))
		{
			return 'item';
		}
		else
		{
			return self::$item;
		}
	}
	// return start index, implicit: 1
	public function GetStartIndex()
	{
		if(is_null(self::$start))
		{
			return 1;
		}
		else
		{
			return self::$start;
		}
	}
	// print help information
	private function printHelp()
	{
		echo "
	*********************************************
	*                                           *
	*  Script converting JSON file to XML file  *
	*  Author: Tomas Aubrecht (xaubre02)        *
	*  IPP: 2016/2017                           *
	*                                           *
	*********************************************

	- Parameters:

	  --help :-> show this help

	  --input=filename :-> filename is the name of input file encoded in UTF-8

	  --output=filename :-> text XML output file encoded in UTF-8 

	  -h=subst :-> invalid characters are replaced with subst 
	           :-> implicit subst is '-'

	  -n -> header will not be generated

	  -r=root-element :-> name of pair element containing result
	                  :-> implicit: result is not encapsulated

	  --array-name=array-element :-> rename element to 'array-element'

	  --item-name=item-element :-> rename element item to 'item-element'
	  
	  -s :-> transform string into text element 

	  -i :-> transform number into text element

	  -l :-> transform literals 'true', 'false' and 'null'
	       into text elements

	  -c :-> activate problematic characters translation

	  -a, --array-size :-> complement array with its size

	  -t, --index-items :-> complement item with index

	  --start=n :-> explicit settings of array index

	  --types :-> display data types of elements
		";
	}


	public function Process(array $argv)
	{
		unset($argv[0]); // Removes the first argument, which is the name of the script
		// go through all input parameters and checks if they are correct
		foreach($argv as $value) // Checks every passed argument
		{
			if($value === '--help')
			{	// if help is already set, exit with error code 1
				if(self::$help)
				{
					forceExit(1, "You typed '--help' more than once!");
				}
				else
				{	// sethelp to be true
					self::$help = TRUE;
				}
			}
			// the rest is same as previous one 
			else if(substr($value, 0, 8) === '--input=')
			{
				if(is_null(self::$inFile))
				{
					self::$inFile = substr($value, 8);
				}
				else
				{
					forceExit(1, "You typed '--input=...' more than once!");
				}
			}
			else if(substr($value, 0, 9) === '--output=')
			{
				if(is_null(self::$outFile))
				{
					self::$outFile = substr($value, 9);
				}
				else
				{
					forceExit(1, "You typed '--output=...' more than once!");
				}
			}
			else if(substr($value, 0, 3) === '-h=')
			{
				if(is_null(self::$subst))
				{
					self::$subst = substr($value, 3);
				}
				else
				{
					forceExit(1, "You typed '-h=...' more than once!");
				}
			}
			else if($value === '-n')
			{
				if(self::$generate)
				{
					self::$generate = FALSE;
				}
				else
				{
					forceExit(1, "You typed '-n' more than once!");
				}
			}
			else if(substr($value, 0, 3) === '-r=')
			{
				if(is_null(self::$root))
				{
					self::$root = substr($value, 3);
					// names can contain letters, digits, hyphens, underscores, and periods they must start with a letter or underscore AND they cannot start with the letters 'xml'
					if ( preg_match('/[^a-ž0-9-_.]/i', self::$root) OR preg_match('/[^a-ž_]/i', substr(self::$root, 0, 1)) OR strtolower(substr(self::$root, 0, 3)) === 'xml' )
					{
						
						forceExit(50, "You typed invalid root name: ".self::$root);
					}
				}
				else
				{
					forceExit(1, "You typed '-r=' more than once!");
				}
			}
			else if(substr($value, 0, 13) === '--array-name=')
			{
				if(is_null(self::$array))
				{
					self::$array = substr($value, 13);
					// names can contain letters, digits, hyphens, underscores, and periods they must start with a letter or underscore AND they cannot start with the letters 'xml'
					if ( preg_match('/[^a-ž0-9-_.]/i', self::$array) OR preg_match('/[^a-ž_]/i', substr(self::$array, 0, 1)) OR strtolower(substr(self::$array, 0, 3)) === 'xml' )
					{
						
						forceExit(50, "You typed invalid array name: ".self::$array);
					}
				}
				else
				{
					forceExit(1, "You typed '--array-name=...' more than once!");
				}
			}
			else if(substr($value, 0, 12) === '--item-name=')
			{
				if(is_null(self::$item))
				{
					self::$item = substr($value, 12);
					// names can contain letters, digits, hyphens, underscores, and periods they must start with a letter or underscore AND they cannot start with the letters 'xml'
					if ( preg_match('/[^a-ž0-9-_.]/i', self::$item) OR preg_match('/[^a-ž_]/i', substr(self::$item, 0, 1)) OR strtolower(substr(self::$item, 0, 3)) === 'xml' )
					{
						
						forceExit(50, "You typed invalid item name: ".self::$item);
					}
				}
				else
				{
					forceExit(1, "You typed '--item-name=...' more than once!");
				}
			}
			else if($value === '-s')
			{
				if(self::$string)
				{
					forceExit(1, "You typed '-s' more than once!");
				}
				else
				{
					self::$string = TRUE;
				}
			}
			else if($value === '-i')
			{
				if(self::$number)
				{
					forceExit(1, "You typed '-i' more than once!");
				}
				else
				{
					self::$number = TRUE;
				}
			}
			else if($value === '-l')
			{
				if(self::$literal)
				{
					forceExit(1, "You typed '-l' more than once!");
				}
				else
				{
					self::$literal = TRUE;
				}
			}
			else if($value === '-c')
			{
				if(self::$convert)
				{
					forceExit(1, "You typed '-c' more than once!");
				}
				else
				{
					self::$convert = TRUE;
				}
			}
			else if($value === '-a' OR $value === '--array-size')
			{
				if(self::$size)
				{
					forceExit(1, "You typed '-a or --array-' more than once!");
				}
				else
				{
					self::$size = TRUE;
				}
			}
			else if($value ==='-t' OR $value === '--index-items')
			{
				if(self::$index)
				{
					forceExit(1, "You typed '-t or --index-...' more than once!");
				}
				else
				{
					self::$index = TRUE;
				}
			}
			else if(substr($value, 0, 8) === '--start=')
			{
				if(is_null(self::$start))
				{
					self::$start = substr($value, 8);
					if(!ctype_digit(strval(self::$start))/*!is_numeric(self::$start) OR self::$start < 0 OR is_float(self::$start)*/)
					{
						forceExit(1, "You typed invalid index! - ".self::$start);
					}
				}
				else
				{
					forceExit(1, "You typed '--start=...' more than once!");
				}
			}
			else if($value === '--types')
			{
				if(self::$types)
				{
					forceExit(1, "You typed '--types' more than once!");
				}
				else
				{
					self::$types = TRUE;
				}
			}
			else if($value === '--padding')
			{
				if(self::$padding)
				{
					forceExit(1, "You typed '--padding' more than once!");
				}
				else
				{
					self::$padding = TRUE;
				}
			}
			else
			{
				forceExit(1, "Invalid parameters.");
			}

		}
		// help must be set as the only parameter
		if(self::$help AND count($argv) > 1)
		{
			forceExit(1, "Invalid parameters. For more information run program with '--help'.");
		}
		// --start must be set only if -t or --index-items is set
		if(self::$start AND !self::$index)
		{
			forceExit(1, "You set '--start=' parametr without 't' or '--index-items'!");
		}
		// --padding must be set only if -t or --index-items is set
		if(self::$padding AND !self::$index)
		{
			forceExit(1, "You set '--padding' parametr without 't' or '--index-items'!");
		}
		// print help if everything else was correctly passed
		if(self::$help)
		{
			self::printHelp();
			forceExit();
		}
	}
}

class Converter
{
	private static
	// used for making indentation
	$gap = 0,
	// used for correct index numbering for array in array 
	$arrayCount = 0, 	// number of arrays and their index
	$arraySize, 		// array size
	$zeros,				// padding size
	$index;				// index number


	public function GenerateXML($input)
	{
		// initialize and check input format
		$input = self::Decode($input);
		// get and validate root name
		$root = Arguments::GetRootName();
		// get a header of XML file
		$result = Arguments::GetHeader();
		// make root element if it is enabled
		if(!is_null($root))
		{
			$result .= '<'.self::CheckName($root).'>'."\n";
			self::$gap = 1;
		}
		// start generating XML file
		$result .= self::RecursiveProcess($input);
		// make ending of root element if it's enabled
		if(!is_null($root))
		{
			$result .= '</'.self::CheckName($root).'>'."\n";
		}
		// return the result XML
		return $result;
	}

	private function RecursiveProcess($input, $name = NULL, $item = FALSE)
	{
		$result = '';
		// if indexing is enabled and if it is an item
		$index = '';
		if(Arguments::$index AND $item)
		{
			$order = self::$index[self::$arrayCount]; // index number
			// if padding is enabled, make it then 
			if(Arguments::$padding)
			{
				$order = str_pad($order, self::$zeros[self::$arrayCount], '0', STR_PAD_LEFT);
			}
			$index .= " index=\"".$order."\"";
			self::$index[self::$arrayCount]++; 
		}
		// Array processing
		if(is_array($input))
		{
			// init new array numbering
			self::$arraySize = count($input); // array size
			self::$index[++self::$arrayCount] = Arguments::GetStartIndex(); // actual array starting index
			self::$zeros[self::$arrayCount] = strlen(Arguments::GetStartIndex() + self::$arraySize - 1); // size of padding of index
			
			$size = '';
			if(Arguments::$size)
			{	// additional attribute size
				$size = " size=\"".self::$arraySize."\"";
			}
			// add element name if there is any
			if(!is_null($name))
			{
				$result = self::MakeIndentation().'<'.self::CheckName($name).$index.'>'."\n";
				self::$gap++;
			}
			// add array name
			$result .= self::MakeIndentation().'<'.self::CheckName(Arguments::GetArrayName()).$size.'>'."\n";
			self::$gap++;
			// proccess each array item
			foreach ($input as $key => $value)
			{
				$result .= self::RecursiveProcess($value, Arguments::GetItemName(), TRUE);
			}
			// add array name's ending
			self::$gap--;
			$result .= self::MakeIndentation().'</'.self::CheckName(Arguments::GetArrayName()).'>'."\n";
			// add element name's ending if there is any
			if(!is_null($name))
			{
				self::$gap--;
				$result .= self::MakeIndentation().'</'.self::CheckName($name).'>'."\n";
			}

			self::$arrayCount--;
		}
		// Object processing
		else if(is_object($input))
		{	// add object name if there is any
			if(!is_null($name))
			{
				$result .= self::MakeIndentation().'<'.self::CheckName($name).$index.'>'."\n";
				self::$gap++;
			}
			// process each item in this object
			foreach ($input as $key => $value) 
			{
				$result .= self::RecursiveProcess($value, $key);
			}
			// add object name's ending if there is any
			if(!is_null($name))
			{
				self::$gap--;
				$result .= self::MakeIndentation().'</'.self::CheckName($name).'>'."\n";
			}
		}
		// Element processing
		else
		{
			$type = '';
			if(Arguments::$types)
			{	// additional attribute indicating which type is element's value
				$type = " type=\"";

				$dataType = gettype($input);
				// transform these cases
				if($dataType === 'boolean' OR $dataType === 'NULL')
				{
					$dataType = 'literal';
				}
				else if($dataType === 'double')
				{
					$dataType = 'real';
				}
				$type .= $dataType."\"";
			}
			// transform to elements if user wishes so
			if( (is_numeric($input) AND Arguments::$number) OR (is_string($input) AND Arguments::$string) )
			{	
				$result = self::MakeIndentation().'<'.self::CheckName($name).$index.$type.'>'."\n";
				self::$gap++;
				$result .= self::MakeIndentation().self::CheckValue($input)."\n";
				self::$gap--;
				$result .= self::MakeIndentation().'</'.self::CheckName($name).'>'."\n";
			}
			// transform to elements if user wishes so
			else if((is_bool($input) OR is_null($input)) AND Arguments::$literal)
			{
				$result  = self::MakeIndentation().'<'.self::CheckName($name).$index.$type.'>'."\n";
				self::$gap++;
				$result .= self::MakeIndentation().'<'.self::CheckValue($input).'/>'."\n";
				self::$gap--;
				$result .= self::MakeIndentation().'</'.self::CheckName($name).'>'."\n";
			}
			// none of previous conditions was set
			else
			{	// create element with attributes
				$result = self::MakeIndentation().'<'.self::CheckName($name).$index." value=\"".self::CheckValue($input)."\"".$type.'/>'."\n";
			}

		}
		return $result;
	}

	// names can contain letters, digits, hyphens, underscores, and periods they must start with a letter or underscore AND they cannot start with the letters 'xml'
	private function CheckName($name)
	{	// replace invalid characters
		$name = preg_replace('/[^a-ž0-9-_.]/i', Arguments::GetSubstitution(), $name);
		// check if new created name is valid
		if ( preg_match('/[^a-ž0-9-_.]/i', $name) OR preg_match('/[^a-ž_]/i', substr($name, 0, 1)) OR strtolower(substr($name, 0, 3)) === 'xml' )
		{
			
			forceExit(51, "Invalid element's name: ".$name);
		}
		// else return corrected name
		return $name;
	}
	// checks and convetrs value parameter
	private function CheckValue($value)
	{
		// NULL convert to string - null
		if(is_null($value))
		{
			return 'null';
		}
		// floors and return float or double value
		else if(is_float($value))
		{
			return floor($value);
		}
		// BOOL value converts to string
		else if(is_bool($value))
		{
			return $value ? 'true' : 'false';
		}
		// special and problematic characters are converted and returned
		else if(Arguments::$convert)
		{
			return htmlspecialchars($value);
		}
		// if none of previous options is set, don't do anything
		else
		{
			return $value;
		}
	}
	// makes output xml file nice and synoptic by making an indentation
	private function MakeIndentation()
	{
		$space = '';

		for ($x = 0; $x < self::$gap; $x++)
		{
    		$space .="\t";
		} 

		return $space;
	}
	// checks if the input file is valid format and decodes it
	private function Decode($content)
	{
		$content = trim($content);
		$content = json_decode($content);

		if(is_null($content))
		{
			forceExit(4, 'Invalid format of input file!');
		}
		else
		{
			return $content;
		}
	}
}
// prints massage on stderr and exits the program
function forceExit($code = 0, $message = '')
{
	fwrite(STDERR, "$message\n");
	exit($code);
}

// ****************************** Begin ****************************** 
Arguments::Process($argv);

$xml = Converter::GenerateXML(file::Read(Arguments::GetInputFile()));

file::Write(Arguments::GetOutputFile(), $xml);
// ******************************* End ******************************* 

?>