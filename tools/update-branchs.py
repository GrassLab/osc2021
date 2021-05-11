import argparse
import typing

import yaml
import git

if __name__ == "__main__":
    parser = argparse.ArgumentParser()

    parser.add_argument('file')

    args = parser.parse_args()

    with open(args.file) as f:
        ids = yaml.safe_load(f)

    # find origin
    repo = git.Repo(".")

    origin = repo.remote(name='origin')
    fetch_infos: typing.List[git.remote.FetchInfo] = origin.fetch()
    source_branch: git.RemoteReference = origin.refs["0856085-example"]
    origin_head = repo.head.ref
    source_branch.checkout()

    def exist_branch(name: str) -> bool:
        for info in fetch_infos:
            if name in info.name:
                return True
        return False

    push_count = 0
    try:
        for idx in ids:
            if exist_branch(idx):
                print(f'{idx} already exists!')
                continue
            print(f'create {idx} & push ...')
            # create branch
            h = repo.create_head(idx)
            # push
            origin.push(idx)
            # remove local
            repo.delete_head(h)
            push_count += 1

        print(f'total: {len(ids)}, push {push_count} branch')
    finally:
        origin_head.checkout()
