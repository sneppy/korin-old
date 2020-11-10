import math
import re
import gdb
from typing import Iterator

def make_template_arguments_iterator(type: gdb.Type):
	"""  """

	try:
		idx = 0
		while True:
			yield type.template_argument(idx)
			idx += 1
	except RuntimeError:
		pass

def array_size(array: gdb.Value):
	"""  """

	return array.type.sizeof // array[0].type.sizeof

def make_node_iterator(node_ptr: gdb.Value):
	"""  """

	# Yield nodes, from left to right
	while node_ptr != 0:
		node = node_ptr.dereference()
		node_ptr = node['next']
		yield node['data']

def make_tree_iterator(tree: gdb.Value):
	"""  """
	
	root_ptr = tree['root']
	if root_ptr == 0: return iter([])

	node_ptr = root_ptr
	node = node_ptr.dereference()

	# Get leftmost node
	while node['left'] != 0:
		node_ptr = node['left']
		node = node_ptr.dereference()
	
	# Yield nodes, from left to right
	yield from make_node_iterator(node_ptr)

class ArrayPrinter:
	"""  """

	class Iterator:
		"""  """

		def __init__(self, begin: gdb.Value, end: gdb.Value) -> None:
			"""  """

			self.item = begin
			self.end = end
			self.idx = 0
		
		def __iter__(self):
			"""  """

			return self
		
		def __next__(self):
			"""  """

			idx = self.idx
			self.idx += 1

			# Reached end of array
			if self.item == self.end:
				raise StopIteration
			
			# Get value and print it
			value = self.item.dereference()
			self.item += 1

			return ('[%d]' % idx, value)

	def __init__(self, typename: str, value: gdb.Value) -> None:
		"""  """

		self._typename = typename
		self._value = value

		self.buffer = value['buffer']
		self.count = value['count']
		self.capacity = value['capacity']
		self.is_string = value.type.template_argument(0).code == gdb.TYPE_CODE_CHAR

	def display_hint(self) -> str:
		"""  """

		return 'array'
	
	def children(self) -> Iterator:
		"""  """
		
		return self.Iterator(self.buffer, self.buffer + self.count)

	def to_string(self) -> str:
		"""  """
		
		if self.is_string:
			# TODO
			return ''
		else:
			return 'Array %s[%d] with max. capacity %d' % (self._value.type.template_argument(0), self.count, self.capacity)

class StringPrinter:
	"""  """

	def __init__(self, typename: str, value: gdb.Value) -> None:
		"""  """

		self._typename = typename
		self._value = value

		self.array = value['array']

	def display_hint(self) -> str:
		"""  """

		return 'string'
	
	def to_string(self) -> str:
		"""  """

		if self.array['buffer'] == 0:
			return ''
		else:
			return self.array['buffer'].string()

class ListPrinter:
	"""  """

	class Iterator:
		"""  """

		def __init__(self, begin: gdb.Value, end: gdb.Value) -> None:
			"""  """

			self.item = begin
			self.end = end
			self.idx = 0
		
		def __iter__(self):
			"""  """

			return self
		
		def __next__(self):
			"""  """

			idx = self.idx
			self.idx += 1

			# Check if we reached end
			if self.item == self.end: raise StopIteration
			
			# Get value and increment iterator
			link = self.item.dereference()
			self.item = link['next']

			return ('[%d]' % idx, link['data'])

	def __init__(self, typename: str, value: gdb.Value) -> None:
		"""  """

		self._typename = typename
		self._value = value

		self.length = value['length']
		self.head_ptr = value['head']
		self.tail_ptr = value['tail']

	def display_hint(self) -> str:
		"""  """

		return 'array'
	
	def children(self):
		"""  """

		if self.head_ptr != 0:
			return self.Iterator(self.head_ptr, self.tail_ptr.dereference()['next'])
		else:
			return []
	
	def to_string(self) -> str:
		"""  """

		return 'List %s[%d]' % (self._value.type.template_argument(0), self.length)

class TuplePrinter:
	"""  """

	def __init__(self, typename: str, value: gdb.Value) -> None:
		"""  """

		self._typename = typename
		self._value = value
		self._tuple_base = value.type.fields()[0]

	def display_hint(self) -> str:
		"""  """

		return 'array'
	
	def children(self) -> Iterator:
		"""  """
		
		base_type = self._value.type.fields()[0]
		base = self._value[base_type]

		idx = 0
		while base is not None:
			fields = base.type.fields()
			if len(fields) == 2:
				# Get item
				item_type = fields[1]
				item = base[item_type]

				# Output item
				yield ('[%d]' % idx, item)

				base_type = fields[0]
				base = base[base_type]
				idx += 1
			else:
				# Get item
				item_type = fields[0]
				item = base[item_type]

				# Output item
				yield ('[%d]' % idx, item)

				base = None
	
	def to_string(self) -> str:
		"""  """

		template_arguments = [str(t) for t in make_template_arguments_iterator(self._value.type)]
		return 'Tuple <%s>[%d]' % (', '.join(template_arguments), len(template_arguments))

class MapPrinter:
	"""  """

	def __init__(self, typename: str, value: gdb.Value) -> None:
		"""  """

		self._typename = typename
		self._value = value
		self._key_type = value.type.template_argument(0)
		self._val_type = value.type.template_argument(1)
		
		self.tree = value['tree']
		self.num_items = self.tree['numNodes']
	
	def display_hint(self) -> str:
		"""  """

		return 'map'
	
	def children(self) -> Iterator:
		"""  """

		idx = 0		
		for pair in make_tree_iterator(self.tree):
			yield ('[%d]' % (idx + 0), pair['first'])
			yield ('[%d]' % (idx + 1), pair['second'])
			idx += 2

	def to_string(self) -> str:
		"""  """

		return 'Map <%s, %s>[%d]' % (self._key_type, self._val_type, self.num_items)

class SetPrinter:
	"""  """

	def __init__(self, typename: str, value: gdb.Value) -> None:
		"""  """

		self._typename = typename
		self._value = value
		self._item_type = value.type.template_argument(0)

		self.tree = value['tree']
		self.size = self.tree['numNodes']
	
	def display_hint(self) -> str:
		"""  """

		return 'array'
	
	def children(self) -> Iterator:
		"""  """

		idx = 0
		for item in make_tree_iterator(self.tree):
			yield ('[%d]' % idx, item)
			idx += 1
	
	def to_string(self) -> str:
		"""  """

		return 'Set %s[%d]' % (self._item_type, self.size)

class VecPrinter:
	"""  """

	def __init__(self, typename: str, value: gdb.Value) -> None:
		"""  """

		self._typename = typename
		self._value = value
		self._vector_type = value.type.template_argument(0)

		self.array = value['array']
		self.size = array_size(self.array)
	
	def to_string(self) -> str:
		"""  """

		return '%s%d%s' % (self._vector_type, self.size, self.array)

class QuatPrinter:
	"""  """

	def __init__(self, typename: str, value: gdb.Value) -> None:
		"""  """

		self._typename = typename
		self._value = value
		
		try:
			a = math.acos(value['w'])
			s = 1.0 / math.sin(a)
			self.angle = a * 2.0
			self.axis = [
				value['x'] * s,
				value['y'] * s,
				value['z'] * s
			]
		except ZeroDivisionError:
			self.angle = 0.0
			self.axis = [0.0, 0.0, 0.0]
	
	def to_string(self) -> str:
		"""  """

		return 'quat<%g rad around {%g, %g, %g}>' % (self.angle, *self.axis)

class SubPrinter(object):
	"""  """

	def __init__(self, name: str, typeprinter) -> None:
		"""  """

		self.name = name
		self.typeprinter = typeprinter
	
	def __call__(self, value: gdb.Value):
		"""  """

		return self.typeprinter(self.name, value)

class Printer(object):
	"""  """

	def __init__(self, name: str) -> None:
		"""  """

		super(Printer, self).__init__()
		self.name = name
		self.subprinters = []
		self.lookup_table = {}
		self.lookup_regex = re.compile('^([\w:]+)(<.*>)?$')
	
	def add(self, name: str, typeprinter) -> None:
		"""  """

		subprinter = SubPrinter(name, typeprinter)
		self.subprinters.append(subprinter)
		self.lookup_table[name] = subprinter

	@staticmethod
	def get_basic_type(type: gdb.Type):
		"""  """

		if type.code == gdb.TYPE_CODE_REF:
			# Dereference type
			type = type.target()
		
		# Get unqualified type, strip typedefs
		type = type.unqualified().strip_typedefs()

		return type.tag
	
	def __call__(self, value: gdb.Value):
		"""  """

		# Get basic typename
		typename = self.get_basic_type(value.type)
		if typename is None: return None

		# Match against lookup regex
		match = self.lookup_regex.match(typename)
		if match is None: return None

		if value.type.code == gdb.TYPE_CODE_REF:
			if hasattr(gdb.Value, 'referenced_value'):
				value = value.referenced_value()

		# Match against lookup table
		try:
			basename = match.group(1)
			subprinter = self.lookup_table[basename]
			return subprinter(value)
		except KeyError:
			# Printer not found, return None
			return None

korin_printer = None

def build_korin_printer() -> None:
	"""  """

	global korin_printer

	korin_printer = Printer('korin')
	korin_printer.add('Array', ArrayPrinter)
	korin_printer.add('StringBase', StringPrinter)
	korin_printer.add('List', ListPrinter)
	korin_printer.add('Tuple', TuplePrinter)
	korin_printer.add('Map', MapPrinter)
	korin_printer.add('Set', SetPrinter)
	korin_printer.add('Vec2', VecPrinter)
	korin_printer.add('Vec3', VecPrinter)
	korin_printer.add('Vec4', VecPrinter)
	korin_printer.add('Quat', QuatPrinter)

def register_korin_printer(obj: gdb.Objfile) -> None:
	"""  """

	global korin_printer

	obj.pretty_printers.append(korin_printer)

build_korin_printer()
register_korin_printer(gdb.current_objfile())
