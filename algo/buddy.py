import math
import sys
from dataclasses import dataclass, field
from typing import List, Optional, Tuple, Iterable

from rich.columns import Columns
from rich.console import Console
from rich.table import Table
from rich.console import RenderGroup
from rich.panel import Panel

console = Console()

# A placeholder for values that is not available
NOT_AVAILABLE = sys.maxsize


def min_exp_statisfy(request: int) -> int:
    """Find the minimal exp of 2 that 2^exp >= requested size """
    pow = 0
    while 2 ** pow < request:
        pow += 1
    return pow


@dataclass
class FrameNode:
    arr_index: int  # index of this frame in array

    # exponent, indicate the length of contiguous memory this node represents
    # if exp >=0:
    #   size_of_memory =  2**exp
    # else:
    #   this FrameNode has been allocated or not at the beginning frame of a contiguous memory
    exp: int

    # link to other node in freelist
    prev: Optional["FrameNode"] = None
    next: Optional["FrameNode"] = None

    def __repr__(self):
        return f"<Frame idx:{self.arr_index}, exp:{self.exp}>"

    def buddy_idx(self):
        """Return the frame index of its buddy"""
        bit_to_invert = 1 << self.exp
        return self.arr_index ^ bit_to_invert

    def push_to_list(self, list: "FrameNode"):
        """Push a node to the back of a list"""
        self.prev = list.prev
        self.next = list
        list.prev.next = self
        list.prev = self

    def remove_from_list(self):
        """Remove a node from list, no need to specify the list becuase it's a double link-list"""
        prev = self.prev
        next = self.next
        prev.next = next
        next.prev = prev

    def list_pop(self) -> "FrameNode":
        """Pop a node from list, assuming that the list is not empty"""
        node = self.prev
        node.remove_from_list()
        return node

    def dump_list(self) -> str:
        """Print the list out
        Caution: must called with the list head, otherwise would produce wrong result
        """
        root = self
        iter = root.next
        s = "<FreeList"
        while iter is not root:
            s += f" ->Node(idx={iter.arr_index}, exp={iter.exp})"
            iter = iter.next
        s += ">"
        return s

    def is_list_empty(self) -> bool:
        """check if a list is empty
        Caution: must called with the list head, otherwise would produce wrong result
        """
        return self.next == self

    @classmethod
    def get_new_list(cls) -> "FrameNode":
        """Return a new node as a list header"""
        node = cls(arr_index=NOT_AVAILABLE, exp=NOT_AVAILABLE)
        node.next, node.prev = node, node
        return node


class UserAllocated:
    """A book keeping structure to know the user holded memory"""

    nodes: List[FrameNode] = []

    @classmethod
    def add(cls, node: FrameNode):
        node.prev, node.next = None, None
        cls.nodes.append(node)

    @classmethod
    def remove(cls, frame_idx: int):
        node = [n for n in cls.nodes if n.arr_index == frame_idx][0]
        cls.nodes.remove(node)


@dataclass
class BuddyAllocater:
    total_frames: int  # Must be an power of 2
    frame_size_in_kb: int = 4

    # max exponent available for this allocator
    max_exp: int = field(init=False)

    # Link-lists which contains of free nodes with same size
    free_lists: List[FrameNode] = field(init=False)
    frame_array: List[FrameNode] = field(init=False)

    def list_for_exp(self, exp: int) -> FrameNode:
        return self.free_lists[exp]

    def __post_init__(self):
        def is_power_of_two(n: int):
            return (n != 0) and (n & (n - 1) == 0)

        if not is_power_of_two(self.total_frames):
            raise RuntimeError(
                f"Number of frames in a buddy system must be a power of 2, get:{self.total_frames}"
            )
        self.max_exp = int(math.log2(self.total_frames))
        self.frame_array = [
            FrameNode(arr_index=i, exp=-1) for i in range(self.total_frames)
        ]
        self.free_lists = [FrameNode.get_new_list() for _ in range(self.max_exp + 1)]
        root_frame: FrameNode = self.frame_array[0]
        root_frame.exp = self.max_exp
        self.frame_array[0].push_to_list(self.list_for_exp(self.max_exp))

    def dump_status(self):
        def make_col(_list: Iterable, title: str):
            return Columns(
                [str(n) for n in _list],
                equal=True,
                align="left",
                title=title,
            )

        frame_arr = make_col(self.frame_array, title="The Frame Array")
        alloc_arr = make_col(UserAllocated.nodes, title="Allocated Frames")

        def make_list_table():
            table = Table("exp", "list", title="Free Lists")
            for i, _list in enumerate(self.free_lists, start=0):
                table.add_row(str(i), _list.dump_list())
            return table

        list_table = make_list_table()
        status = RenderGroup(list_table, frame_arr, alloc_arr)
        console.print(Panel(status, title="Buddy Status"))

    def allocate(self, size_in_byte: int) -> int:
        """Return the starting address of the contiguous memory requested"""
        num_frame_requested = math.ceil(size_in_byte / self.frame_size_in_kb)
        target_exp = min_exp_statisfy(num_frame_requested)
        console.print(
            f"[yellow]Allocate Request[reset] size:{size_in_byte}, exp:{target_exp}, frame_size:{2**target_exp}"
        )

        if target_exp >= self.max_exp:
            raise RuntimeError("Bad input")

        success = self._provide_frame_with_exp(target_exp)
        if success:
            node = self.list_for_exp(target_exp).list_pop()
            UserAllocated.add(node)
            return node.arr_index * self.frame_size_in_kb
        else:
            console.print("[red] no space for allocation")
            return -1

    def free(self, addr: int):
        frame_idx = addr // self.frame_size_in_kb

        UserAllocated.remove(frame_idx=frame_idx)
        console.print(
            f"[bold green]Free Request[reset] addr:{addr}, frame_idx:{frame_idx}"
        )

        while True:
            node = self.frame_array[frame_idx]
            if node.buddy_idx() >= self.total_frames:
                break
            buddy = self.frame_array[node.buddy_idx()]
            # Buddy is currently not in any list, therefore in used
            msg = (
                "Try to merge "
                + f"buddy(idx:{buddy.arr_index},exp:{buddy.exp})-"
                + f"node(idx:{node.arr_index},exp:{node.exp})"
            )
            if buddy.next is None:
                node.push_to_list(self.list_for_exp(node.exp))
                msg += f" [red]busy[reset], recycle node: {node}"
                console.log(msg)
                break

            def node_in_order(
                a: FrameNode, b: FrameNode
            ) -> Tuple[FrameNode, FrameNode]:
                """Node with lower arr_index comes first"""
                return (a, b) if a.arr_index < b.arr_index else (b, a)

            # Buddy is available, merge both node
            msg += f" [green]merged"
            buddy.remove_from_list()

            low, high = node_in_order(node, buddy)
            high.exp = -1
            low.exp += 1
            frame_idx = low.arr_index
            console.log(msg)
            # console.log(f"try merge next layer, idx:{frame_idx}, exp:{low.exp}")

    def _provide_frame_with_exp(self, required_exp: int) -> bool:
        """Makesure there exists a frame which's exp == required_exp"""

        # The request have already been fulfilled
        if not self.list_for_exp(required_exp).is_list_empty():
            return True

        def find_upmost_exp_to_split() -> Optional[int]:
            target = required_exp
            found = False
            while target <= self.max_exp:
                if not self.list_for_exp(target).is_list_empty():
                    found = True
                    break
                target += 1
            return target if found else None

        upmost_exp_to_split = find_upmost_exp_to_split()
        if upmost_exp_to_split is None:
            return False

        # Splitting Nodes until we have a node with sutiable size
        msg = "Split node from list(exp):"
        for exp in range(upmost_exp_to_split, required_exp, -1):
            msg += f" {exp}"
            node = self.list_for_exp(exp).list_pop()

            child_exp: int = exp - 1
            child1 = self.frame_array[node.arr_index]
            child2 = self.frame_array[node.arr_index + 2 ** child_exp]
            child1.exp, child2.exp = child_exp, child_exp
            child1.push_to_list(self.list_for_exp(child_exp))
            child2.push_to_list(self.list_for_exp(child_exp))
        console.log(msg)

        return True


def main():
    buddy = BuddyAllocater(total_frames=8)
    buddy.dump_status()
    a = buddy.allocate(4)
    b = buddy.allocate(9)
    buddy.dump_status()
    buddy.free(b)
    buddy.free(a)
    buddy.dump_status()


if __name__ == "__main__":
    main()
