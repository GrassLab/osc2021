from typing import List, Optional

from rich.console import Console

console = Console()


class Info:
    num_tree_layers = 4

    arr_size = 2 ** num_tree_layers
    frame_size_in_kb = 4

    @classmethod
    def show(cls):
        console.print(f"layers: [yellow]{Info.num_tree_layers}")
        console.print(f"framesize: [yellow]{Info.arr_size}")


def min_exp_statisfy(request_size: int) -> int:
    """Find the minimal exp of 2 that 2^exp >= request_size """
    pow = 0
    while 2 ** pow < request_size:
        pow += 1
    if pow > Info.num_tree_layers:
        raise RuntimeError(
            f"Request allocation size({request_size}) is larger than max_limit:{Info.arr_size}"
        )
    return pow


class FreeNode:
    def __init__(self, size_exp: int, idx: int, next=None):
        self.size_exp = size_exp
        self.size = 2 ** size_exp
        self.idx = idx
        self.next = next

    def __repr__(self):
        return f"(Node idx:{self.idx}, size:{self.size}, next->{self.next})"


class FreeList:
    def __init__(self):
        self.root = None
        self.count = 0

    def __repr__(self):
        return f"{self.root}"

    def push(self, node: FreeNode):
        node.next = self.root
        self.root = node
        self.count += 1

    def pop(self) -> FreeNode:
        if self.count > 0:
            node = self.root
            self.root = self.root.next
            self.count -= 1
            return node
        raise RuntimeError("pop from empty list")

    def try_remove(self, frame_idx: int) -> Optional[FreeNode]:
        """pop a free node with frame_idx"""
        iter, prev, found = self.root, None, None
        while iter is not None:
            if iter.idx == frame_idx:
                if prev is not None:
                    prev.next = iter.next
                if iter is self.root:
                    self.root = self.root.next
                found = iter
                self.count -= 1
                break
            prev, iter = iter, iter.next
        return found


class FreeLists:
    def __init__(self, total: int):
        self.children: List[FreeList] = [FreeList() for _ in range(total)]

    def for_exp(self, exp: int) -> FreeList:
        return self.children[exp]

    def show(self):
        console.print("FreeLists: [")
        indent1 = " " * 10
        indent2 = " " * 6
        for idx, n in enumerate(self.children):
            console.print(indent1 + f"{idx}: {n}")
        console.print(indent2 + "]")


class UserMem:
    """A book keeping structure to know the user holded memory"""

    def __init__(self):
        self.allocated: List[FreeNode] = []

    def add(self, node: FreeNode):
        node.next = None
        self.allocated.append(node)

    def remove(self, frame_idx: int):
        node = [n for n in self.allocated if n.idx == frame_idx][0]
        self.allocated.remove(node)

    def show(self):
        console.print("allocated: ", self.allocated)


class DS:
    def __init__(self):
        self.arr = [-1 for _ in range(Info.arr_size)]
        self.arr[0] = Info.num_tree_layers

        self.free_lists = FreeLists(total=Info.num_tree_layers + 1)
        exp = Info.num_tree_layers
        self.free_lists.for_exp(exp).push(FreeNode(size_exp=exp, idx=0))

        self._user: UserMem = UserMem()

    def show_arr(self):
        console.print(f"arr: {self.arr}")

    def show_status(self):
        self.show_arr()
        self.free_lists.show()
        self._user.show()

    def have_node_with_exp(self, exp: int) -> bool:
        return self.free_lists.for_exp(exp).count > 0

    def request_provide(self, required_exp: int) -> bool:
        """Return a idx to node with required_exp"""

        def do_split(exp: int):
            node = self.free_lists.for_exp(exp).pop()

            def create_node(idx: int, size_exp: int):
                # add new node to list
                node = FreeNode(size_exp=size_exp, idx=idx)
                self.free_lists.for_exp(size_exp).push(node)
                # update up array
                self.arr[idx] = size_exp

            child_exp = node.size_exp - 1
            create_node(idx=node.idx, size_exp=child_exp)
            create_node(idx=node.idx + 2 ** child_exp, size_exp=child_exp)
            return True

        def find_root_exp_to_split():
            root_exp_to_split = required_exp
            while root_exp_to_split < Info.num_tree_layers:
                if self.have_node_with_exp(root_exp_to_split):
                    break
                root_exp_to_split += 1

            # no node for splitting, we've use all of our space
            if self.free_lists.for_exp(root_exp_to_split).count == 0:
                return None
            return root_exp_to_split

        if required_exp > Info.num_tree_layers:
            return False
        # if already have
        if self.have_node_with_exp(required_exp):
            return True
        root_exp_to_split = find_root_exp_to_split()
        if root_exp_to_split is None:
            return False
        print(f"root to split: {root_exp_to_split}")
        for exp in range(root_exp_to_split, required_exp, -1):
            print(f"split:{exp}")
            do_split(exp=exp)
        return True

    def free(self, frame_idx: int):

        # book keeping
        self._user.remove(frame_idx=frame_idx)

        def get_buddy_idx(idx: int, exp: int):
            bit_to_invert = 1 << exp
            return idx ^ bit_to_invert

        cur_idx = frame_idx
        while True:
            cur_exp = self.arr[cur_idx]
            bidx = get_buddy_idx(cur_idx, cur_exp)
            node = self.free_lists.for_exp(cur_exp).try_remove(frame_idx=bidx)
            if node is None:
                # buddy is not available, push current node to list
                node = FreeNode(size_exp=cur_exp, idx=cur_idx)
                self.free_lists.for_exp(cur_exp).push(node)
                print(f"push node to free-list: {node}")
                break
            # buddy is available, merge it
            small_idx = min(cur_idx, bidx)
            big_idx = max(cur_idx, bidx)
            self.arr[big_idx] = -1
            self.arr[small_idx] = cur_exp + 1
            cur_idx = small_idx
            print(f"try merge next layer, idx:{cur_idx}, exp:{self.arr[cur_idx]}")
        print(f"idx of buddy:{bidx}")
        print(f"freenode: {node}")

    def allocate(self, size: int):
        """Return the index of the node that contains available space to allocate"""
        pow = min_exp_statisfy(size)
        success = self.request_provide(pow)
        if success:
            node = self.free_lists.for_exp(pow).pop()

            # book-keeping
            self._user.add(node=node)
            node.next = None

            return node.idx
        else:
            print("failed")
            return None


def allocate(ds: DS, size: int) -> int:
    """ Return the start address of the allocable memory"""
    ds.show_status()
    console.print(f"[bold red]request allocate size: {size}")
    idx = ds.allocate(size)
    ds.show_status()
    if idx is None:
        console.print("[red]" + "=" * 30)
        return None
    addr = idx * 4
    console.print(f"address with least continuous size:{size} -> {addr}K")
    console.print("[red]" + "=" * 30)
    return addr


def free(ds: DS, addr: int):
    frame_idx = addr // 4
    ds.show_status()
    console.print(f"[bold red]free frame:{frame_idx}")
    ds.free(frame_idx=frame_idx)
    ds.show_status()
    console.print("[red]" + "=" * 30)


def main():
    Info.show()
    ds = DS()

    a = allocate(ds, 3)
    b = allocate(ds, 2)
    c = allocate(ds, 1)

    free(ds, a)
    free(ds, b)
    free(ds, c)


if __name__ == "__main__":
    main()
