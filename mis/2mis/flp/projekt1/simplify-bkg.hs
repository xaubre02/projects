-- ------------------------
-- Subject: FLP 2018/2019
-- Project: simplify-bkg
-- Date:    2019-02-16
-- Author:  Tomas Aubrecht
-- Login:   xaubre02
-- ------------------------

-- Imports
import System.Environment         -- getArgs
import System.IO                  -- handles
import qualified Data.Set as Set  -- working with sets
import Text.Printf                -- printing functions
import Data.List                  -- intercalate, map, lines, ..
import Data.Char                  -- isAlpha, isUpper, ..


-- ---------- CUSTOM RECORDS -----------

-- record representing a rewrite rule in format: A -> alpha
data Rule = Rule
  {
    left :: Char,    -- A: nonterminal
    right :: [Char]  -- alpha: in (N u T)*
  } deriving (Show, Eq, Ord)

-- record representing a grammar in format: G = (N, T, S, P)
data Grammar = Grammar
  {
    nonterminals :: Set.Set Char,  -- N: a finite set of nonterminals
    terminals :: Set.Set Char,     -- T: a finite set of terminals
    startSymbol :: Char,           -- S: the start symbol
    rules :: Set.Set Rule          -- P: a finite set of rewrite rules
  }


-- ---------- PARSE FUNCTIONS ----------

-- remove spaces from a string
removeSpaces :: String -> String
removeSpaces [] = []
removeSpaces (' ':xs) = removeSpaces (xs)
removeSpaces (x:xs) = x : removeSpaces xs

-- split a string into separate parts by a comma
splitStr :: String -> [String]
splitStr string = split string []
  where
    split [] t = [t]
    split (x:xs) t = if x == ','
      then t : split xs []
      else split xs (t ++ [x])

-- check whether all elements in a list are unique - the list is a set
isSet :: [String] -> Bool
isSet = all ( (==) 1 . length) . group . sort

-- parse nonterminals
parseNonterminals :: String -> Set.Set Char
parseNonterminals string =
  -- each part has to be a valid unique nonterminal symbol
  if isSet parts && all isNonterminal parts
    then Set.fromList (map head parts)
    else error "Nonterminals are not valid! Has to be a set of letters A-Z."
    where
      parts = splitStr (removeSpaces string)
      -- check whether the string is a nonterminal
      isNonterminal :: String -> Bool
      isNonterminal string = (length string) == 1 && isAlpha (head string) && isUpper (head string)

-- parse terminals
parseTerminals :: String -> Set.Set Char
parseTerminals string =
  -- each part has to be a valid unique nonterminal symbol
  if isSet parts && all isTerminal parts
    then Set.fromList (map head parts)
    else error "Terminals are not valid! Has to be a set of letters a-z."
    where
      parts = splitStr (removeSpaces string)
      -- check whether the string is a terminal
      isTerminal :: String -> Bool
      isTerminal string = (length string) == 1 && isAlpha (head string) && isLower (head string)

-- parse a start symbol from a string
parseStartSymbol :: String -> Set.Set Char -> Char
parseStartSymbol string nonterminals =
  -- start symbol has to be one of the nonterminals
  if (length string) == 1 && elem (head string) nonterminals
    then (head string)
    else error "Start symbol has to be one of the nonterminals!"

-- parse a single rewrite rule
parseRule :: String -> Set.Set Char -> Set.Set Char -> Rule
parseRule rule nonterms terms =
  -- NONTERM -> alpha, where alpha in (NONTERMS u TERMS)*
  if (length rule) >= 4 && elem (rule !! 0) nonterms && rule !! 1 == '-' && rule !! 2 == '>' && ((validAlpha (drop 3 rule) nonterms terms) || (drop 3 rule) == "#")
    then Rule (rule !! 0) (drop 3 rule)
    else error ("'" ++ rule ++ "' is an invalid rule!")
    where
      -- each symbol of alpha has to be either in nonterms or terms
      validAlpha alpha nonterms terms = all (isIn nonterms terms) alpha
      isIn nonterms terms char = elem char nonterms || elem char terms

-- parse a list of rewrite rules
parseRules :: [String] -> Set.Set Char -> Set.Set Char -> Set.Set Rule
parseRules rules nonterms terms = 
  if isSet(rules) 
    then Set.fromList (createList rules [])
    else error "Rules are not unique!"
    where
      createList [] list = list
      createList (x:xs) list = createList xs (list ++ [parseRule (removeSpaces x) nonterms terms])

-- parse input and return a grammar
parseInput :: [String] -> Grammar
parseInput lines =
  if (length lines) < 4
    then error "Invalid grammar on input! At least 4 lines needed!"  -- a grammar has to be a quaternion
    else Grammar nonterms terms (parseStartSymbol (lines !! 2) nonterms) (parseRules (drop 3 lines) nonterms terms)
    where
      nonterms = parseNonterminals (lines !! 0)
      terms    = parseTerminals (lines !! 1)


-- ---------- PRINT FUNCTIONS ----------

-- print a grammar
printGrammar :: Grammar -> IO()
printGrammar grammar = do
  printCharSet (nonterminals grammar) -- print nonterminals
  printCharSet (terminals grammar)    -- print terminals
  printOneChar (startSymbol grammar)  -- print start symbol
  printRuleSet (rules grammar)        -- print rules
  where
    -- print a set of chars
    printCharSet :: Set.Set Char -> IO()
    printCharSet list = putStrLn (intersperse ',' (Set.toList (list)))
    -- print a single char
    printOneChar :: Char -> IO()
    printOneChar char = printf "%c\n" char
    -- print a set of rules
    printRuleSet :: Set.Set Rule -> IO()
    printRuleSet rules = mapM_ printRule rules
    -- print a single rule
    printRule :: Rule -> IO()
    printRule rule = printf "%c->%s\n" (left rule) (right rule)


-- ------- SIMPLIFYING FUNCTIONS -------

-- check whether a string is terminating or not
isTerminating :: String -> Set.Set Char -> Bool
isTerminating string terminatings = string == "#" || (all (isTerm terminatings) string)
  where
    -- check whether a char is terminating symbol or terminal
    isTerm terminatings char = elem char terminatings

-- get Ni set of the 4.1 algorithm
getNiSet :: Set.Set Char -> [Rule] -> Set.Set Char
getNiSet terms [] = Set.fromList []
-- 'A->alpha' check whether the alpha consists of terminating symbols only
getNiSet terms (x:xs) = if isTerminating (right x) terms
  then Set.union (Set.fromList [left x]) (getNiSet terms xs)
  else getNiSet terms xs

-- get the resulting Nt set of the 4.1 algorithm
getNtSet :: Set.Set Char -> Set.Set Char -> Set.Set Char -> [Rule] -> Set.Set Char
getNtSet new prev terminals rules = if new == prev
  then new
  -- params: Ni u Sigma(Terminals), rules
  else getNtSet (getNiSet (Set.union new terminals) rules) new terminals rules

-- filter out rules that contains symbols that are not in the provided symbol set
filterRules :: [Rule] -> Set.Set Char -> Set.Set Rule
filterRules [] symbols = Set.fromList []
filterRules (x:xs) symbols = if isIncluded x symbols
  then Set.union (Set.fromList [x]) (filterRules xs symbols)
  else filterRules xs symbols
  where
    -- both left and right side consists of symbols in provided set of symbols 
    isIncluded rule symbols = elem (left rule) symbols && isTerminating (right rule) symbols

-- remove all nonterminating symbols (Algorithm 4.1)
removeNonterminatingSymbols :: Grammar -> Grammar
removeNonterminatingSymbols grammar = if elem (startSymbol grammar) setNt -- check if L(G) is not empty
  then Grammar setNt (terminals grammar) (startSymbol grammar) (filterRules (Set.toList (rules grammar)) (Set.union setNt (terminals grammar)))
  else error "Grammar produces an empty language!"
  where
    setNt = getNtSet (getNiSet (terminals grammar) ruleList) (Set.fromList []) (terminals grammar) ruleList
    ruleList = Set.toList (rules grammar)

-- get one step of the Vi of the 4.2 algorithm
getViStep :: Set.Set Char -> [Rule] -> Set.Set Char
getViStep derivatives [] = Set.fromList []
-- 'A->alpha' add all symbols in alpha if the A is a derivative of the start symbol
getViStep derivatives (x:xs) = if elem (left x) derivatives
  then Set.union (addAlpha (right x)) (getViStep derivatives xs)
  else getViStep derivatives xs
  where
    addAlpha [] = Set.fromList []
    addAlpha (x:xs) = Set.union (Set.fromList [x]) (addAlpha xs)

-- get the resulting Vi set of the 4.2 algorithm
getViSet :: Set.Set Char -> Set.Set Char -> [Rule] -> Set.Set Char
getViSet new prev rules = if new == prev
  then new
  else getViSet (Set.union (getViStep (Set.union new prev) rules) new) new rules

-- remove all unreachable symbols (Algorithm 4.2)
removeUnreachableSymbols :: Grammar -> Grammar
-- params: Vi n Nt, Vi n T, start symbol, filtered rules (n = intersection)
removeUnreachableSymbols grammar = Grammar (Set.intersection setVi (nonterminals grammar)) (Set.intersection setVi (terminals grammar)) (startSymbol grammar) (filterRules (Set.toList (rules grammar)) setVi)
  where
    -- params: V1 u V0, V0, rules
    setVi = getViSet (Set.union (getViStep (Set.fromList [startSymbol grammar]) ruleList) (Set.fromList [startSymbol grammar])) (Set.fromList [startSymbol grammar]) ruleList
    ruleList = Set.toList (rules grammar)

-- remove all useless symbols (Algorithm 4.3)
removeUselessSymbols :: Grammar -> Grammar
removeUselessSymbols grammar = removeUnreachableSymbols (removeNonterminatingSymbols grammar)

-- ----------- MAIN FUNCTION -----------

main = do
  -- program arguments
  args <- getArgs
  -- mandatory argument not provided
  if (length args) == 0
    then error "Too few arguments!"
    -- too many arguments provided
    else if (length args > 2)
      then error "Too many arguments!"
      else do
        -- get input
        input <- if (length args) == 2
          then readFile (head (tail args))
          else hGetContents stdin

        -- first argument
        case(head args) of
          "-i" -> printGrammar (parseInput (lines input))
          "-1" -> printGrammar (removeNonterminatingSymbols (parseInput (lines input)))
          "-2" -> printGrammar (removeUselessSymbols (parseInput (lines input)))
          otherwise -> error ("Unknown argument: " ++ (head args))
